#include <utility>

#include <QtCore/QProcess>

#include "TomControl.h"

TomControl::TomControl(QString gotimePath, bool bashScript, QObject *parent) : QObject(parent),
                                                                               _gotimePath(std::move(gotimePath)),
                                                                               _bashScript(bashScript) {

    // updates our project cache
    loadProjects();

    // cache status for the initial value
    status();
    loadRecentProjects();

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this] {
        status();
        loadRecentProjects();
    });
    timer->start(30 * 1000);
}

void TomControl::cacheProjects(const QList<Project> &projects) {
    //fixme sync on mutex?
    _cachedProjects.clear();
    for (const auto &project : projects) {
        _cachedProjects[project.getID()] = project;
    }
}

QList<Project> TomControl::cachedRecentProjects() const {
    return _cachedRecentProjects;
}

QList<Project> TomControl::loadProjects(int max) {
    QStringList args = QStringList() << "projects"
                                     << "--name-delimiter=||"
                                     << "-f"
                                     << "fullName,id,parentID,hourlyRate";
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
        if (lineItems.size() == 4) {
            const auto &names = lineItems.at(0).split("||");
            const auto &id = lineItems.at(1);
            const auto &parent = lineItems.at(2);
            const auto &hourlyRate = lineItems.at(3);

            result.append(Project(names, id, parent, hourlyRate));
        }
    }

    if (max <= 0) {
        cacheProjects(result);
    }
    return result;
}

bool TomControl::isStarted(const Project &project, bool includeSubprojects) {
    if (!project.isValid()) {
        return false;
    }
    bool active = _activeProject.getID() == project.getID();
    if (!active && includeSubprojects) {
        // fixme we could optimize this with a mapping parent -> children
        for (const auto &p : _cachedProjects.values()) {
            if (p.getParentID() == project.getID()) {
                bool started = isStarted(p, true);
                if (started) {
                    return started;
                }
            }
        }
    }
    return active;
}

bool TomControl::startProject(const Project &project) {
    auto success = run(QStringList() << "start" << project.getID()).isSuccessful();
    if (success) {
        auto stopped = _activeProject;
        status();
        loadRecentProjects();

        emit projectStatusChanged(project, stopped);
    }
    return success;
}

bool TomControl::cancelActivity() {
    const TomStatus &active = status();

    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        status();
        loadRecentProjects();

        emit projectStatusChanged(Project(), active.currentProject());
    }
    return success;
}

bool TomControl::stopActivity() {
    const TomStatus &current = status();

    _activeProject = Project();
    //fixme handle failed update
    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        status();
        loadRecentProjects();

        emit projectStatusChanged(Project(), current.currentProject());
    }
    return success;
}

TomStatus TomControl::cachedStatus() {
    return _cachedStatus;
}

TomStatus TomControl::status() {
    QStringList args = QStringList() << "status"
                                     << "--name-delimiter=||"
                                     << "-f" << "id,projectFullName,projectID,projectParentID,startTime";

    CommandStatus status = run(args);
    TomStatus result = TomStatus();
    if (status.isSuccessful()) {
        // fixme atm we omly support a single active project
        QStringList lines = status.stdoutContent.split("\n", QString::SkipEmptyParts);
        if (!lines.isEmpty()) {
            QStringList parts = lines.first().split("\t");
            if (parts.size() != 5) {
                qWarning() << "unexpected status line" << lines.first();
                return TomStatus();
            }

            const QStringList &projectName = parts[1].split("||");
            const QString &projectID = parts[2];
            const QString &parentID = parts[3];

            QDateTime startTime = QDateTime::fromString(parts[4], Qt::ISODate);
            Project project = Project(projectName, projectID, parentID, "");
            result = TomStatus(true, project, startTime);
        }
    }

//    bool changed = _cachedStatus != result;
    _cachedStatus = result;
    _activeProject = _cachedStatus.currentProject();

//    if (changed) {
//        emit statusChanged();
//    }

    return result;
}

QList<Frame *> TomControl::loadFrames(const QString &projectID, bool includeSubprojects, bool includeArchived) {
    QStringList args = QStringList() << "frames"
                                     << "-o" << "json"
                                     << "-p" << projectID
                                     << "-f" << "id,projectID,startTime,stopTime,lastUpdated,archived,notes";

    if (includeSubprojects) {
        args.append("--subprojects");
    }

    args.append(QString("--archived=%1").arg(includeArchived ? "true" : "false"));

    const CommandStatus &resp = run(args);
    if (resp.isFailed()) {
        qDebug() << "frame command failed";
        return QList<Frame *>();
    }

    const QByteArray &stdout = resp.stdoutContent.toUtf8();
    if (stdout.isEmpty()) {
        return QList<Frame *>();
    }

    QJsonParseError err = QJsonParseError();
    QJsonDocument json = QJsonDocument::fromJson(stdout, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "json parse error" << err.errorString();
        return QList<Frame *>();
    }

    QList<Frame *> result;
    if (!json.isArray()) {
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
        const QString notes = item["notes"].toString("");
        const QStringList tags = QStringList(); // fixme
        const bool archived = item["archived"].toBool(false);

        // fixme who's deleting the allocated data?
        result.append(new Frame(id, nestedProjectID, start, end, lastUpdated, notes, tags, archived));
    }
    return result;
}

bool TomControl::renameProject(const QString &id, const QString &newName) {
    return updateProjects(QStringList() << id, true, newName, false, "", false, "");
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
                             bool updateProject, const QString &projectID,
                             bool updateArchived, bool archived) {
    if (frames.isEmpty()) {
        return false;
    }

    QStringList frameIDs;
    QStringList projectIDs;

    for (const auto *frame : frames) {
        frameIDs << frame->id;
        projectIDs << frame->projectID;
    }

    return updateFrames(frameIDs, projectIDs, updateStart, start, updateEnd, end,
                        updateNotes, notes,
                        updateProject, projectID,
                        updateArchived, archived);
}

bool TomControl::updateFrames(const QStringList &ids, const QStringList &projectIDs,
                              bool updateStart, const QDateTime &start,
                              bool updateEnd, const QDateTime &end,
                              bool updateNotes, const QString &notes,
                              bool updateProject, const QString &projectID,
                              bool updateArchived, bool archived) {
    QStringList args;
    args << "edit" << "frame";
    if (updateStart) {
        args << "--start" << start.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODateWithMs);
    }
    if (updateEnd) {
        args << "--end" << end.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODateWithMs);
    }
    if (updateNotes) {
        args << "--notes" << notes;
    }
    if (updateProject) {
        args << "--project" << projectID;
    }
    if (updateArchived) {
        args << QString("--archived=%1").arg(archived ? "true" : "false");
    }
    args << ids;

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success) {
        if (updateProject) {
            emit framesMoved(ids, projectIDs, projectID);
        }
        if (updateArchived) {
            emit framesArchived(ids, projectIDs, archived);
        }

        emit framesUpdated(ids, projectIDs);
    }

    return success;
}

bool TomControl::updateProjects(const QStringList &ids, bool updateName, const QString &name, bool updateParent, const QString &parentID, bool updateHourlyRate,
                                const QString &hourlyRate, bool signalHierarchyChange) {
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
    if (updateHourlyRate) {
        args << "--hourly-rate" << hourlyRate;
    }
    args << ids;

    CommandStatus status = run(args);
    bool success = status.isSuccessful();
    if (success) {
        // fixme find smarter way to update a single project? needs to handle hierarchy changes
        loadProjects();

        QList<Project> updatedProjects;
        for (const auto &id : ids) {
            const Project &p = cachedProject(id);
            if (p.isValid()) {
                updatedProjects << p;
            }
        }

        if (signalHierarchyChange) {
            emit projectHierarchyChanged(updatedProjects);
        }

        // signal changes to project after the hierarchy change as the receivers need this order
        for (const auto &p: updatedProjects) {
            emit projectUpdated(p);
        }
    }
    return success;
}

const ProjectsStatus TomControl::projectsStatus(const QString &overallID, bool includeActive, bool includeArchived) {
    QString idList = "id,trackedDay,totalTrackedDay,trackedYesterday,totalTrackedYesterday,trackedWeek,totalTrackedWeek,trackedMonth,totalTrackedMonth,trackedYear,totalTrackedYear,trackedAll,totalTrackedAll";
    const int expectedColumns = idList.count(',') + 1;

    QStringList args;
    args << "status" << "projects" << "-f" << idList;
    if (includeActive) {
        args << "--include-active";
    }
    if (!overallID.isEmpty()) {
        args << "--show-overall" << overallID;
    }
    args << QString("--archived=%1").arg(includeArchived ? "true" : "false");

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

        Timespan yesterday = Timespan(parts.takeFirst().toLongLong());
        Timespan yesterdayTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan week = Timespan(parts.takeFirst().toLongLong());
        Timespan weekTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan month = Timespan(parts.takeFirst().toLongLong());
        Timespan monthTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan year = Timespan(parts.takeFirst().toLongLong());
        Timespan yearTotal = Timespan(parts.takeFirst().toLongLong());

        Timespan all = Timespan(parts.takeFirst().toLongLong());
        Timespan allTotal = Timespan(parts.takeFirst().toLongLong());

        mapping.insert(id, ProjectStatus(id, all, allTotal,
                                         year, yearTotal,
                                         month, monthTotal,
                                         week, weekTotal,
                                         yesterday, yesterdayTotal,
                                         day, dayTotal));
    }

    return ProjectsStatus(mapping);
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

        const Project &newProject = Project(names, id, parent, "");
        _cachedProjects[id] = newProject;
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
        _cachedProjects.remove(project.getID());
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
        emit framesRemoved(ids, projectIDs);
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

bool TomControl::isChildProject(const QString &id, const QString &parentID) const {
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

bool TomControl::isAnyChildProject(const QStringList &ids, const QString &parentID) const {
    if (ids.isEmpty() || parentID.isEmpty()) {
        return false;
    }

    for (const auto &id : ids) {
        for (auto p = cachedProject(id); p.isValid(); p = cachedProject(p.getParentID())) {
            if (p.getID() == parentID) {
                return true;
            }
        }
    }
    return false;
}

bool TomControl::isAnyParentProject(const QString &id, const QStringList &parents) const {
    for (const auto &parentID : parents) {
        if (isChildProject(id, parentID)) {
            return true;
        }
    }
    return false;
}

QStringList TomControl::projectIDs(const QString &projectID, bool includeSubprojects) const {
    if (!_cachedProjects.contains(projectID)) {
        return QStringList();
    }

    QStringList ids;
    ids << projectID;

    if (includeSubprojects) {
        for (const auto &childID : _cachedProjects.keys()) {
            if (isChildProject(childID, projectID)) {
                ids << childID;
            }
        }
    }

    return ids;
}

QString TomControl::htmlReport(const QString &outputFile,
                               const QStringList &projectIDs,
                               bool includeSubprojects,
                               QDate start, QDate end,
                               TimeRoundingMode frameRoundingMode, int frameRoundingMinutes,
                               QStringList splits, const QString &templateID,
                               bool matrixTables,
                               bool showEmpty,
                               bool showSummary,
                               bool includeArchived,
                               const QString &title, const QString &description,
                               bool showSales,
                               bool showTracked, bool showUntracked) {
    QStringList args;
    args << "report";
    if (!outputFile.isEmpty()) {
        args << QString("--output-file=%1").arg(outputFile);
    }
    args << "--split=" + splits.join(",");

    if (!projectIDs.isEmpty()) {
        for (const auto &id : projectIDs) {
            args << "--project=" + id;
        }
    }

    args << QString("--subprojects=%1").arg(includeSubprojects ? "true" : "false");

    if (!templateID.isEmpty()) {
        args << "--template=" + templateID;
    }

    if (start.isValid() && !start.isNull()) {
        QDateTime startDate = QDateTime(start);
        startDate.setTime(QTime(0, 0, 0, 0));
        args << "--from=" + startDate.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }

    if (end.isValid() && !end.isNull()) {
        QDateTime endDate = QDateTime(end);
        endDate.setTime(QTime(23, 59, 50, 999));
        args << "--to=" + endDate.toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
    }

    args << QString("--matrix-tables=%1").arg(matrixTables ? "true" : "false");
    args << QString("--show-empty=%1").arg(showEmpty ? "true" : "false");
    args << QString("--show-summary=%1").arg(showSummary ? "true" : "false");
    args << QString("--include-archived=%1").arg(includeArchived ? "true" : "false");
    args << QString("--show-sales=%1").arg(showSales ? "true" : "false");
    args << QString("--show-tracked=%1").arg(showTracked ? "true" : "false");
    args << QString("--show-untracked=%1").arg(showUntracked ? "true" : "false");


    if (!title.isEmpty()) {
        args << "--title=" + title;
    }

    if (!description.isEmpty()) {
        args << "--description=" + description;
    }

    switch (frameRoundingMode) {
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

    const auto &status = run(args, 5000);

    return status.stdoutContent;
}

const Project TomControl::cachedActiveProject() const {
    return _activeProject;
}

bool TomControl::hasSubprojects(const Project &project) {
    if (project.isRootProject()) {
        return !_cachedProjects.isEmpty();
    }

    const QString &parentID = project.getID();
    for (const auto &p : _cachedProjects) {
        if (p.getParentID() == parentID) {
            return true;
        }
    }
    return false;
}

void TomControl::archiveProjectFrames(const Project &project, bool includeSubprojects) {
    if (!project.isValid()) {
        return;
    }

    QStringList args;
    args << "frames" << "archive" << "--project" << project.getID();
    if (includeSubprojects) {
        args << "--include-subprojects";
    }

    const CommandStatus &status = run(args);
    if (status.isSuccessful()) {
        auto ids = projectIDs(project.getID(), includeSubprojects);
        emit projectFramesArchived(ids);
    }
}

CommandStatus TomControl::run(const QStringList &args, long timeoutMillis) {
    if (_gotimePath.isEmpty()) {
        return CommandStatus("", "executable name is empty", -1);
    }

    QMutexLocker lock(&_mutex);

    auto start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (args.first() != "status") {
        qDebug() << "running" << _gotimePath;
        qDebug() << "running" << _gotimePath << args;
    }

    QProcess process(this);
    if (_bashScript) {
        process.start("/usr/bin/bash", QStringList() << _gotimePath << args);
    } else {
        process.start(_gotimePath, args);
    }
    process.waitForFinished(timeoutMillis);

    QString output(process.readAllStandardOutput());
    QString errOutput(process.readAllStandardError());

    if (process.exitCode() != 0) {
        qDebug() << "exit code:" << process.exitCode() << "stdout:" << output << "stderr" << errOutput;
    }
    if (args.first() != "status") {
        qDebug() << "tom command:" << (QDateTime::currentDateTime().toMSecsSinceEpoch() - start) << "ms";
    }
    return CommandStatus(output, errOutput, process.exitCode());
}

QList<Project> TomControl::loadRecentProjects() {
    _cachedRecentProjects = loadProjects(5);
    return _cachedRecentProjects;
}

CommandStatus TomControl::version() {
    return run(QStringList() << "--version");
}
