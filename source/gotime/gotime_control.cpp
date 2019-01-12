#include <utility>

#include <QtCore/QProcess>

#include "gotime_control.h"

gotime_control::gotime_control(const QString gotimePath, bool bashScript, QObject *parent) :
        _gotimePath(gotimePath), _bashScript(bashScript), QObject(parent) {

    const gotime_status &status = this->status();
    if (status.isValid) {
        _activeProjectID = status.currentProject().getID();
    }
}

QList<Project> gotime_control::loadProjects() {
    command_status status = run(QStringList() << "projects"
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
            const auto &name = lineItems.at(0);
            const auto &id = lineItems.at(1);
            const auto &parent = lineItems.at(2);

            result.append(Project(name, id, parent));
        }
    }
    return result;
}

bool gotime_control::isStarted(Project &project) {
    return _activeProjectID == project.getID();
}

bool gotime_control::startProject(Project &project) {
    auto success = run(QStringList() << "start" << project.getName()).isSuccessful();
    if (success) {
        _activeProjectID = project.getID();
        emit projectStarted(project);
    }
    return success;
}

bool gotime_control::cancelActivity() {
    const gotime_status &active = status();

    _activeProjectID = "";
    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        emit projectCancelled(active.currentProject());
        emit projectStopped(active.currentProject());
    }
    return success;
}

bool gotime_control::stopActivity() {
    const gotime_status &current = status();

    _activeProjectID = "";
    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        emit projectStopped(current.currentProject());
    }
    return success;
}

gotime_status gotime_control::status() {
    // frameID, projectName, projectID, ...
    command_status status = run(
            QStringList() << "status" << "-f" << "id,projectFullName,projectID,projectParentID,startTime");
    if (status.isFailed()) {
        return gotime_status();
    }

    // fixme atm we omly support a single active project
    QStringList lines = status.stdoutContent.split("\n");
    if (lines.isEmpty()) {
        return gotime_status();
    }

    QStringList parts = lines.first().split("\t");
    if (parts.size() != 5) {
        qDebug() << "unexpected status line" << lines.first();
        return gotime_status();
    }

    QDateTime startTime = QDateTime::fromString(parts[4], Qt::ISODate);
    Project project = Project(parts[1], parts[2], parts[3]);
    return gotime_status(true, project, startTime);
}

QList<Frame *> gotime_control::loadFrames(QString projectID, bool includeSubprojects) {
    QStringList args =
            QStringList() << "frames" << "-p" << projectID << "-f" << "id,startTime,stopTime,lastUpdated,notes";
    if (includeSubprojects) {
        args.append("--subprojects");
    }

    const command_status &resp = run(args);
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

command_status gotime_control::run(QStringList &args) {
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

    return command_status(output, errOutput, process.exitCode());
}

const ProjectsStatus gotime_control::projectsStatus() {
    QStringList args;
    args << "status" << "projects" << "-f"
         << "id,trackedDay,totalTrackedDay,trackedWeek,totalTrackedWeek,trackedMonth,totalTrackedMonth,trackedYear,totalTrackedYear";

    command_status cmdStatus = run(args);
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

        QString id = parts[0];

        Timespan day = Timespan(parts[0].toLongLong());
        Timespan dayTotal = Timespan(parts[1].toLongLong());

        Timespan week = Timespan(parts[2].toLongLong());
        Timespan weekTotal = Timespan(parts[3].toLongLong());

        Timespan month = Timespan(parts[4].toLongLong());
        Timespan monthTotal = Timespan(parts[5].toLongLong());

        Timespan year = Timespan(parts[6].toLongLong());
        Timespan yearTotal = Timespan(parts[7].toLongLong());

        mapping.insert(id, ProjectStatus(id, year, yearTotal, month, monthTotal, week, weekTotal, day, dayTotal));
    }

    qDebug() << mapping.size();
    return ProjectsStatus(mapping);
}