#include <utility>

#include <QtCore/QProcess>

#include "GotimeControl.h"

GotimeControl::GotimeControl(const QString gotimePath, bool bashScript, QObject *parent) :
        _gotimePath(gotimePath), _bashScript(bashScript), QObject(parent) {

    const GotimeStatus &status = this->status();
    if (status.isValid) {
        _activeProjectID = status.currentProject().getID();
    }
}

QList<Project> GotimeControl::loadProjects() {
    CommandStatus status = run(QStringList() << "projects"
                                             << "--name-delimiter=||"
                                             << "-f"
                                             << "fullName,id,parentID");
    if (status.isFailed()) {
        return QList<Project>();
    }

    QStringList lines = status.stdoutContent.split("\n", QString::SkipEmptyParts);
    QList<Project> result;
    for (const auto &line : lines) {
        QStringList lineItems = line.split("\t");
        if (lineItems.size() == 3) {
            const auto &names = lineItems.at(0).split("||");
            const auto &id = lineItems.at(1);
            const auto &parent = lineItems.at(2);

            result.append(Project(names, id, parent));
        }
    }
    return result;
}

bool GotimeControl::isStarted(Project &project) {
    return _activeProjectID == project.getID();
}

bool GotimeControl::startProject(Project &project) {
    auto success = run(QStringList() << "start" << project.getName()).isSuccessful();
    if (success) {
        _activeProjectID = project.getID();
        emit projectStarted(project);
    }
    return success;
}

bool GotimeControl::cancelActivity() {
    const GotimeStatus &active = status();

    _activeProjectID = "";
    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        emit projectCancelled(active.currentProject());
        emit projectStopped(active.currentProject());
    }
    return success;
}

bool GotimeControl::stopActivity() {
    const GotimeStatus &current = status();

    _activeProjectID = "";
    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        emit projectStopped(current.currentProject());
    }
    return success;
}

GotimeStatus GotimeControl::status() {
    // frameID, projectName, projectID, ...
    QStringList args = QStringList() << "status"
                                     << "--name-delimiter=||"
                                     << "-f" << "id,projectFullName,projectID,projectParentID,startTime";

    CommandStatus status = run(args);
    if (status.isFailed()) {
        return GotimeStatus();
    }

    // fixme atm we omly support a single active project
    QStringList lines = status.stdoutContent.split("\n");
    if (lines.isEmpty()) {
        return GotimeStatus();
    }

    QStringList parts = lines.first().split("\t");
    if (parts.size() != 5) {
        qDebug() << "unexpected status line" << lines.first();
        return GotimeStatus();
    }

    const QStringList &projectName = parts[1].split("||");
    const QString &projectID = parts[2];
    const QString &parentID = parts[3];
    QDateTime startTime = QDateTime::fromString(parts[4], Qt::ISODate);

    Project project = Project(projectName, projectID, parentID);
    return GotimeStatus(true, project, startTime);
}

QList<Frame *> GotimeControl::loadFrames(QString projectID, bool includeSubprojects) {
    QStringList args = QStringList() << "frames"
                                     << "-p" << projectID
                                     << "-f" << "id,startTime,stopTime,lastUpdated,notes";

    if (includeSubprojects) {
        args.append("--subprojects");
    }

    const CommandStatus &resp = run(args);
    if (resp.isFailed()) {
        qDebug() << "frame command failed";
        return QList<Frame *>();
    }

    const QStringList &lines = resp.stdoutContent.split("\n");
    if (lines.isEmpty()) {
        qDebug() << "frame command returned empty list";
        return QList<Frame *>();
    }

    QList<Frame *> result;
    for (const auto &line: lines) {
        if (line.isEmpty()) {
            qDebug() << "frame command output contains empty line";
            continue;
        }

        QStringList cols = line.split("\t");
        if (cols.size() != 5) {
            qDebug() << "invalid number of columns" << line;
            break;
        }

        const QString id = cols[0];
        const QDateTime start = QDateTime::fromString(cols[1], Qt::ISODate);
        const QDateTime end = QDateTime::fromString(cols[2], Qt::ISODate);
        const QDateTime lastUpdated = QDateTime::fromString(cols[3], Qt::ISODate);
        const QString notes = cols[4];
        const QStringList tags = QStringList();

        // fixme who's deleting the allocated data?
        result.append(new Frame(id, projectID, start, end, lastUpdated, notes, tags));
    }
    qDebug() << "loadFrames() returned " << result.size() << "frames";
    return result;
}

CommandStatus GotimeControl::run(QStringList &args) {
    qDebug() << "running" << _gotimePath << args;

    QProcess process(this);
    if (_bashScript) {
        process.start("/usr/bin/bash", QStringList() << _gotimePath << args);
    } else {
        process.start(_gotimePath, args);
    }
    process.waitForFinished(600);

    QString output(process.readAllStandardOutput());
    QString errOutput(process.readAllStandardError());

    qDebug() << "exit code:" << process.exitCode();
//    qDebug() << "exit status:" << process.exitStatus();
//    qDebug() << "exit error:" << process.errorString();
//    qDebug() << "stdout:" << output;
//    qDebug() << "err stdout:" << errOutput;

    return CommandStatus(output, errOutput, process.exitCode());
}

const ProjectsStatus GotimeControl::projectsStatus() {
    QStringList args;
    args << "status" << "projects" << "-f"
         << "id,trackedDay,totalTrackedDay,trackedWeek,totalTrackedWeek,trackedMonth,totalTrackedMonth,trackedYear,totalTrackedYear";

    CommandStatus cmdStatus = run(args);
    if (cmdStatus.isFailed()) {
        return ProjectsStatus();
    }

    auto mapping = QHash<QString, ProjectStatus>();

    QStringList lines = cmdStatus.stdoutContent.split("\n", QString::SkipEmptyParts);
    for (const auto &line : lines) {
        QStringList parts = line.split("\t");

        qDebug() << parts;
        if (parts.size() != 9) {
            qDebug() << "unexpected number of columns in" << line;
            continue;
        }

        QString id = parts.takeFirst();

        Timespan day = Timespan(parts.takeFirst().toLongLong());
        Timespan dayTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan week = Timespan(parts.takeFirst().toLongLong());
        Timespan weekTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan month = Timespan(parts.takeFirst().toLongLong());
        Timespan monthTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan year = Timespan(parts.takeFirst().toLongLong());
        Timespan yearTotal = Timespan(parts.takeFirst().toLongLong());

        mapping.insert(id, ProjectStatus(id, year, yearTotal, month, monthTotal, week, weekTotal, day, dayTotal));
    }

    qDebug() << mapping.size();
    return ProjectsStatus(mapping);
}