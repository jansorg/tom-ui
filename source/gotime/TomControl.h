#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore>
#include <QtCore/QString>
#include <data/Frame.h>

#include "data/Project.h"
#include "CommandStatus.h"
#include "GotimeStatus.h"
#include "ProjectStatus.h"

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

    QList<Project> loadRecentProjects(int max);

    QList<Project> cachedProjects() const;

    const Project cachedProject(const QString &id) const;

    GotimeStatus status();

    const ProjectsStatus projectsStatus(const QString &overallID);

    bool isStarted(const Project &project);

    QList<Frame *> loadFrames(QString projectID, bool includeSubprojects);

    bool renameProject(QString id, QString newName);

    bool renameTag(QString id, QString newName);

    bool updateFrame(Frame *frame, bool updateStart, QDateTime start, bool updateEnd, QDateTime end, bool updateNotes, QString notes);

    bool updateFrame(const QString& id, const QString&projectID, bool updateStart, QDateTime start, bool updateEnd, QDateTime end, bool updateNotes, const QString&notes);

    bool removeFrame(Frame frame);

    bool updateProjects(const QStringList &ids, bool updateName, const QString &name, bool updateParent, const QString &parentID);

    bool importMacTimeTracker(const QString &filename);

    bool importFanurioCSV(const QString &filename);

    bool importWatsonFrames(const QString &filename);

    void resetAll();

signals:

    void dataResetNeeded();

    void projectStarted(const Project &);

    void projectStopped(const Project &);

    void projectCancelled(const Project &);

    void projectUpdated(const Project &project);

    void projectCreated(const Project &project);

    void frameUpdated(const QString &id, const QString &projectID);

    void frameRemoved(const QString &id, const QString &projectID);

public slots:

    bool startProject(const Project &project);

    bool stopActivity();

    bool cancelActivity();

private:
    CommandStatus run(const QStringList &args);

    Project _activeProject;
    QHash<QString, Project> _cachedProjects;

    QString _gotimePath;
    bool _bashScript;

    void cacheProjects(const QList<Project> &projects);
};

#endif //GOTIME_UI_GOTIMECONTROL_H
