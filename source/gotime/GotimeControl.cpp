#include <utility>

#include <QtCore/QProcess>

#include "GotimeControl.h"

GotimeControl::GotimeControl(const QString gotimePath, bool bashScript, QObject *parent) : _gotimePath(gotimePath),
                                                                                           _bashScript(bashScript),
                                                                                           QObject(parent) {

    // updates our project cache
    loadProjects();

    const GotimeStatus &status = this->status();
    if (status.isValid) {
        _activeProject = status.currentProject();
    }
}

void GotimeControl::cacheProjects(const QList<Project> &projects) {
    _cachedProjects.clear();
    for (const auto &project : projects) {
        _cachedProjects[project.getID()] = project;
    }
}

QList<Project> GotimeControl::loadRecentProjects(int max) {
    return this->loadProjects(max);
}

QList<Project> GotimeControl::loadProjects(int max) {
    QStringList args = QStringList() << "projects"
                                     << "--name-delimiter=||"
                                     << "-f"
                                     << "fullName,id,parentID";
    if (max > 0) {
        args << "--recent" << QString::number(max);
    }

    CommandStatus status = run(args);
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

    if (max == 0) {
        cacheProjects(result);
    }
    return result;
}

bool GotimeControl::isStarted(const Project &project) {
    return _activeProject.getID() == project.getID();
}

bool GotimeControl::startProject(const Project &project) {
    auto success = run(QStringList() << "start" << project.getName()).isSuccessful();
    if (success) {
        auto stopped = _activeProject;
        _activeProject = project;

        emit projectStarted(project);
        emit projectUpdated(project);

        if (stopped.isValid()) {
            emit projectStopped(stopped);
            emit projectUpdated(stopped);
        }
    }
    return success;
}

bool GotimeControl::cancelActivity() {
    _activeProject = Project();;

    const GotimeStatus &active = status();

    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        emit projectCancelled(active.currentProject());
        emit projectUpdated(active.currentProject());
    }
    return success;
}

bool GotimeControl::stopActivity() {
    const GotimeStatus &current = status();

    _activeProject = Project();
    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        emit projectStopped(current.currentProject());
        emit projectUpdated(current.currentProject());
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
    QStringList lines = status.stdoutContent.split("\n", QString::SkipEmptyParts);
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
                                     << "-o" << "json"
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

    QJsonDocument json(QJsonDocument::fromJson(resp.stdoutContent.toUtf8()));

    QList<Frame *> result;
    if (!json.isArray()) {
        qDebug() << "output not a JSON array" << json;
        return result;
    }

    const QJsonArray &items = json.array();

    for (const auto &arrayItem: items) {
        if (!arrayItem.isObject()) {
            qDebug() << "item not an object" << arrayItem;
            break;
        }

        const QJsonObject &item = arrayItem.toObject();
        const QString id = item["id"].toString();
        const QDateTime start = QDateTime::fromString(item["startTime"].toString(), Qt::ISODate);
        const QDateTime end = QDateTime::fromString(item["stopTime"].toString(), Qt::ISODate);
        const QDateTime lastUpdated = QDateTime::fromString(item["lastUpdated"].toString(), Qt::ISODate);
        const QString notes = item["notes"].toString();
        const QStringList tags = QStringList();

        // fixme who's deleting the allocated data?
        result.append(new Frame(id, projectID, start, end, lastUpdated, notes, tags));
    }
    return result;
}

bool GotimeControl::renameProject(QString id, QString newName) {
    QStringList args;
    args << "rename" << "project" << id << newName;

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success) {
        const Project &project = _cachedProjects.value(id);
        if (project.isValid()) {
            emit projectUpdated(project);
        }
    }

    return success;
}

bool GotimeControl::renameTag(QString id, QString newName) {
    QStringList args;
    args << "rename" << "tag" << id << newName;

    CommandStatus status = run(args);
    return status.isSuccessful();
}

bool GotimeControl::updateFrame(Frame *frame,
                                bool updateStart, QDateTime start,
                                bool updateEnd, QDateTime end,
                                bool updateNotes,
                                QString notes) {
    return updateFrame(frame->id, frame->projectID, updateStart, start, updateEnd, end, updateNotes, notes);
}

bool GotimeControl::updateFrame(QString id, QString projectID,
                                bool updateStart, QDateTime start,
                                bool updateEnd, QDateTime end,
                                bool updateNotes,
                                QString notes) {
    QStringList args;
    args << "edit" << id;
    if (updateStart) {
        args << "--start" << start.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }
    if (updateEnd) {
        args << "--end" << end.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }
    if (updateNotes) {
        args << "--notes" << notes;
    }

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success && !projectID.isEmpty()) {
        const Project &project = _cachedProjects.value(projectID);
        if (project.isValid()) {
            emit projectUpdated(project);
        }
    }
    return success;
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
    return CommandStatus(output, errOutput, process.exitCode());
}
