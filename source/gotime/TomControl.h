#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore>
#include <QtCore/QString>
#include <data/Frame.h>

#include "data/Project.h"
#include "CommandStatus.h"
#include "TomStatus.h"
#include "ProjectStatus.h"

enum TimeRoundingMode {
    NONE, UP, NEAREST, DOWN
};

class TomControl : public QObject {
Q_OBJECT

public:
    explicit TomControl(QString gotimePath, bool bashScript, QObject *parent);

    /**
     * Create a new project.
     * @param project
     * @return  The new project. If the create call failed, then the returned project will return false for Project::isValid()
     */
    Project createProject(const QString &parentID, const QString &name);

    QList<Project> loadProjects(int max = -1);

    QList<Project> loadRecentProjects();

    QList<Project> cachedRecentProjects() const;

    QList<Project> cachedProjects() const;

    const Project cachedProject(const QString &id) const;

    const Project cachedActiveProject() const;

    bool hasSubprojects(const Project &project);

    TomStatus status();

    TomStatus cachedStatus();

    const ProjectsStatus projectsStatus(const QString &overallID, bool includeActive, bool includeArchived);

    bool isStarted(const Project &project, bool includeSubprojects = false);

    QList<Frame *> loadFrames(const QString &projectID, bool includeSubprojects, bool includeArchived);

    bool renameProject(const QString &id, const QString &newName);

    bool removeProject(const Project &project);

    bool renameTag(const QString &id, const QString &newName);

    bool updateFrame(const QList<Frame *> &frames, bool updateStart, const QDateTime &start, bool updateEnd,
                     const QDateTime &end, bool updateNotes, const QString &notes, bool updateProject, const QString &projectID, bool updateArchived, bool archived);

    bool updateFrame(const QStringList &ids, const QString &currentProjectID, bool updateStart, const QDateTime &start, bool updateEnd,
                     const QDateTime &end, bool updateNotes, const QString &notes, bool updateProject, const QString &projectID, bool updateArchived, bool archived);

    bool removeFrames(const QList<Frame *> &frames);

    void archiveProjectFrames(const Project &project, bool includeSubprojects);

    bool updateProjects(const QStringList &ids, bool updateName, const QString &name, bool updateParent, const QString &parentID, bool updateHourlyRate, const QString &hourlyRate);

    bool importMacTimeTracker(const QString &filename);

    bool importFanurioCSV(const QString &filename);

    bool importWatsonFrames(const QString &filename);

    void resetAll();

    bool isChildProject(const QString &id, const QString &parentID);

    QString htmlReport(const QString &outputFile,
                       const QStringList &projectIDs,
                       bool includeSubprojects,
                       QDate start, QDate end,
                       TimeRoundingMode frameRoundingMode, int frameRoundingMinutes, QStringList splits, const QString &templateID,
                       bool matrixTables,
                       bool showEmpty,
                       bool showSummary,
                       bool includeArchived,
                       const QString &title, const QString &description,
                       bool showSales);

signals:

    void dataResetNeeded();

//    void statusChanged();

    void projectStarted(const Project &);

    void projectStopped(const Project &);

    void projectCancelled(const Project &);

    void projectUpdated(const Project &project);

    void projectCreated(const Project &project);

    void projectRemoved(const Project &project);

    void framesUpdated(const QStringList &id, const QString &projectID);

    void framesRemoved(const QStringList &ids, const QString &projectID);

    void framesMoved(const QStringList &ids, const QString &oldProjectID, const QString &newProjectID);

    void framesArchived(const QStringList &projectIDs);

public slots:

    bool startProject(const Project &project);

    bool stopActivity();

    bool cancelActivity();

private:
    CommandStatus run(const QStringList &args, long timeoutMillis = 1000);

    Project _activeProject;
    QHash<QString, Project> _cachedProjects;
    QList<Project> _cachedRecentProjects;
    TomStatus _cachedStatus;

    QString _gotimePath;
    bool _bashScript;
    QMutex _mutex;

    void cacheProjects(const QList<Project> &projects);
};

#endif //GOTIME_UI_GOTIMECONTROL_H
