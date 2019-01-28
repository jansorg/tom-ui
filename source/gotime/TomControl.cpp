#include <utility>

#include <utility>

#include <QtCore/QProcess>

#include "TomControl.h"

TomControl::TomControl(QString gotimePath, bool bashScript, QObject *parent) : QObject(parent),
                                                                               _gotimePath(std::move(gotimePath)),
                                                                               _bashScript(bashScript) {

    // updates our project cache
    loadProjects();

    const GotimeStatus &status = this->status();
    if (status.isValid) {
        _activeProject = status.currentProject();
    }
}

void TomControl::cacheProjects(const QList<Project> &projects) {
    //fixme sync on mutex?
    _cachedProjects.clear();
    for (const auto &project : projects) {
        _cachedProjects[project.getID()] = project;
    }
}

QList<Project> TomControl::loadRecentProjects(int max) {
    return this->loadProjects(max);
}

QList<Project> TomControl::loadProjects(int max) {
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

    if (max <= 0) {
        cacheProjects(result);
    }
    return result;
}

bool TomControl::isStarted(const Project &project) {
    return project.isValid() && _activeProject.getID() == project.getID();
}

bool TomControl::startProject(const Project &project) {
    auto success = run(QStringList() << "start" << project.getID()).isSuccessful();
    if (success) {
        auto stopped = _activeProject;
        _activeProject = project;

        emit projectStarted(project);
        emit projectUpdated(project);

        if (stopped.isValid()) {
            emit projectStopped(stopped);
            if (stopped != _activeProject) {
                emit projectUpdated(stopped);
            }
        }
    }
    return success;
}

bool TomControl::cancelActivity() {
    _activeProject = Project();;

    const GotimeStatus &active = status();

    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        emit projectCancelled(active.currentProject());
        emit projectUpdated(active.currentProject());
    }
    return success;
}

bool TomControl::stopActivity() {
    const GotimeStatus &current = status();

    _activeProject = Project();
    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        emit projectStopped(current.currentProject());
        emit projectUpdated(current.currentProject());
    }
    return success;
}

GotimeStatus TomControl::status() {
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

QList<Frame *> TomControl::loadFrames(const QString &projectID, bool includeSubprojects) {
    QStringList args = QStringList() << "frames"
                                     << "-o" << "json"
                                     << "-p" << projectID
                                     << "-f" << "id,projectID,startTime,stopTime,lastUpdated,notes";

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
        const QString nestedProjectID = item["projectID"].toString();
        const QDateTime start = QDateTime::fromString(item["startTime"].toString(), Qt::ISODate);
        const QDateTime end = QDateTime::fromString(item["stopTime"].toString(), Qt::ISODate);
        const QDateTime lastUpdated = QDateTime::fromString(item["lastUpdated"].toString(), Qt::ISODate);
        const QString notes = item["notes"].toString();
        const QStringList tags = QStringList();

        // fixme who's deleting the allocated data?
        result.append(new Frame(id, nestedProjectID, start, end, lastUpdated, notes, tags));
    }
    return result;
}

bool TomControl::renameProject(const QString &id, const QString &newName) {
    return updateProjects(QStringList() << id, true, newName, false, "");
}

bool TomControl::renameTag(const QString &id, const QString &newName) {
    QStringList args;
    args << "rename" << "tag" << id << newName;

    CommandStatus status = run(args);
    return status.isSuccessful();
}

bool TomControl::updateFrame(const QList<Frame *> &frames,
                             bool updateStart, const QDateTime &start,
                             bool updateEnd, const QDateTime &end,
                             bool updateNotes, const QString &notes,
                             bool updateProject, const QString &projectID) {
    // for now we expect that all frames belong to the same project
    if (frames.isEmpty()) {
        return false;
    }

    QStringList frameIDs;

    const Frame *first = frames.constFirst();
    for (const auto *frame : frames) {
        if (frame->projectID != first->projectID) {
            qDebug() << "more than one project used in frame list";
            return false;
        }
        frameIDs << frame->id;
    }

    return updateFrame(frameIDs, projectID, updateStart, start, updateEnd, end, updateNotes, notes, updateProject,
                       projectID);
}

bool TomControl::updateFrame(const QStringList &ids, const QString &currentProjectID,
                             bool updateStart, const QDateTime &start,
                             bool updateEnd, const QDateTime &end,
                             bool updateNotes, const QString &notes,
                             bool updateProject, const QString &projectID) {
    QStringList args;
    args << "edit" << "frame";
    if (updateStart) {
        args << "--start" << start.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }
    if (updateEnd) {
        args << "--end" << end.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }
    if (updateNotes) {
        args << "--notes" << notes;
    }
    if (updateProject) {
        args << "--project" << projectID;
    }
    args << ids;

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success) {
        // fixme handle update of proejct id (remove and updated?)
        if (!projectID.isEmpty()) {
            if (!currentProjectID.isEmpty()) {
                emit framesMoved(ids, currentProjectID, projectID);
            }
        } else if (!currentProjectID.isEmpty()) {
            emit framesUpdated(ids, currentProjectID);
        }
    }
    return success;
}

bool TomControl::updateProjects(const QStringList &ids, bool updateName, const QString &name, bool updateParent,
                                const QString &parentID) {
    if (ids.isEmpty() || (!updateName && !updateParent)) {
        return true;
    }

    QStringList args;
    args << "edit" << "project";
    if (updateName) {
        args << "--name" << name;
    }
    if (updateParent) {
        args << "--parent" << parentID;
    }
    args << ids;

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success) {
        // fixme find smarter way to update a single project? needs to handle hierarchy changes
        loadProjects();

        for (const auto &id : ids) {
            const Project &p = cachedProject(id);
            if (p.isValid()) {
                emit projectUpdated(p);
            }
        }
    }
    return success;
}

const ProjectsStatus TomControl::projectsStatus(const QString &overallID) {
    QString idList = "id,trackedDay,totalTrackedDay,trackedWeek,totalTrackedWeek,trackedMonth,totalTrackedMonth,trackedYear,totalTrackedYear,trackedAll,totalTrackedAll";
    const int expectedColumns = idList.count(',') + 1;

    QStringList args;
    args << "status" << "projects" << "-f"
         << idList;
    if (!overallID.isEmpty()) {
        args << "--show-overall" << overallID;
    }

    CommandStatus cmdStatus = run(args);
    if (cmdStatus.isFailed()) {
        return ProjectsStatus();
    }

    auto mapping = QHash<QString, ProjectStatus>();

    QStringList lines = cmdStatus.stdoutContent.split("\n", QString::SkipEmptyParts);
    for (const auto &line : lines) {
        QStringList parts = line.split("\t");
        if (parts.size() != expectedColumns) {
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

        Timespan all = Timespan(parts.takeFirst().toLongLong());
        Timespan allTotal = Timespan(parts.takeFirst().toLongLong());

        mapping.insert(id, ProjectStatus(id, all, allTotal, year, yearTotal, month, monthTotal, week, weekTotal, day,
                                         dayTotal));
    }

    return ProjectsStatus(mapping);
}

CommandStatus TomControl::run(const QStringList &args) {
    auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();
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

    if (process.exitCode() != 0) {
        qDebug() << "exit code:" << process.exitCode() << "stdout:" << output << "stderr" << errOutput;
    }
    qDebug() << "tom command:" << (QDateTime::currentDateTime().toMSecsSinceEpoch() - start) << "ms";
    return CommandStatus(output, errOutput, process.exitCode());
}

Project TomControl::createProject(const QString &parentID, const QString &name) {
    QStringList args;
    args << "create" << "project"
         << "--output" << "json"
         << name;

    if (!parentID.isEmpty()) {
        args << "-p" << parentID;
    }

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        QJsonDocument json(QJsonDocument::fromJson(status.stdoutContent.toUtf8()));

        const QJsonObject &item = json.object();
        const QString id = item["id"].toString();
        const QString parent = item["parent"].toString();

        QStringList names;
        for (auto v : item["fullName"].toArray()) {
            names << v.toString();
        }

        const Project &newProject = Project(names, id, parent);
        emit projectCreated(newProject);

        return newProject;
    }

    return Project();
}

bool TomControl::removeProject(const Project &project) {
    QStringList args;
    args << "remove" << "project" << project.getID();

    auto status = run(args);
    if (status.isSuccessful()) {
        emit projectRemoved(project);
    }
    return status.isSuccessful();
}

bool TomControl::removeFrames(const QList<Frame *> &frames) {
    QStringList ids;
    QStringList projectIDs;
    for (auto frame : frames) {
        ids << frame->id;
        projectIDs << frame->projectID;
    }

    QStringList args;
    args << "remove" << "frame" << ids;

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        //fixme allow more than one project
        emit framesRemoved(ids, projectIDs.first());
    }
    return status.isSuccessful();
}

bool TomControl::importMacTimeTracker(const QString &filename) {
    QStringList args;
    args << "import" << "macTimeTracker" << filename;

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        emit dataResetNeeded();
    }
    return status.isSuccessful();
}

bool TomControl::importFanurioCSV(const QString &filename) {
    QStringList args;
    args << "import" << "fanurio" << filename;

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        emit dataResetNeeded();
    }
    return status.isSuccessful();
}

bool TomControl::importWatsonFrames(const QString &filename) {
    QStringList args;
    args << "import" << "watson" << filename;

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        emit dataResetNeeded();
    }
    return status.isSuccessful();
}

void TomControl::resetAll() {
    QStringList args;
    args << "remove" << "all" << "all";

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        emit dataResetNeeded();
    }
}

QList<Project> TomControl::cachedProjects() const {
    return _cachedProjects.values();
}

const Project TomControl::cachedProject(const QString &id) const {
    return _cachedProjects.value(id);
}

bool TomControl::isChildProject(const QString &id, const QString &parentID) {
    if (id.isEmpty() || parentID.isEmpty()) {
        return false;
    }

    for (auto p = cachedProject(id); p.isValid(); p = cachedProject(p.getParentID())) {
        if (p.getID() == parentID) {
            return true;
        }
    }
    return false;
}

QString TomControl::htmlReport(QStringList projectIDs,
                               QDate start, QDate end,
                               TimeRoundingMode frameRoundingMode, int frameRoundingMinutes,
                               QStringList splits, QString templateID) {
    QStringList args;
    args << "report";
    args << "--split" << splits.join(",");

    if (!projectIDs.isEmpty()) {
        args << "--project" << projectIDs;
    }

    if (!templateID.isEmpty()) {
        args << "--template" << templateID;
    }

    if (start.isValid() && !start.isNull()) {
        args << "--from" << QDateTime(start).toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }

    if (end.isValid() && !end.isNull()) {
        args << "--to" << QDateTime(end).toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }

    switch (frameRoundingMode){
        case NONE:
            break;
        case NEAREST:
            args << "--round-frames" << "nearest";
            break;
        case UP:
            args << "--round-frames" << "up";
            break;
        case DOWN:
            break;
    }

    if (frameRoundingMode == NONE) {
        args << "--round-frames-to" << "0m";
    } else {
        args << "--round-frames-to" << QString("%1m").arg(frameRoundingMinutes);
    }

    const auto &status = run(args);

    return status.stdoutContent;
}
