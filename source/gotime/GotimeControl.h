#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore>
#include <QtCore/QString>
#include <data/frame.h>

#include "data/Project.h"
#include "CommandStatus.h"
#include "GotimeStatus.h"
#include "projectstatus.h"

class GotimeControl : public QObject {
Q_OBJECT

public:
    explicit GotimeControl(QString gotimePath, bool bashScript, QObject *parent);

    /**
     * Create a new project.
     * @param project
     * @return  The new project. If the create call failed, then the returned project will return false for Project::isValid()
     */
    Project createProject(const QString& parentID, const QString& name);

    QList<Project> loadProjects(int max = -1);

    QList<Project> loadRecentProjects(int max);

    GotimeStatus status();

    const ProjectsStatus projectsStatus();

    bool isStarted(const Project &project);

    QList<Frame *> loadFrames(QString projectID, bool includeSubprojects);

    bool renameProject(QString id, QString newName);

    bool renameTag(QString id, QString newName);

    bool updateFrame(Frame *frame, bool updateStart, QDateTime start, bool updateEnd, QDateTime end, bool updateNotes, QString notes);

    bool updateFrame(QString id, QString projectID, bool updateStart, QDateTime start, bool updateEnd, QDateTime end, bool updateNotes, QString notes);

signals:

    void projectStarted(const Project &);

    void projectStopped(const Project &);

    void projectCancelled(const Project &);

    void projectUpdated(const Project &project);


public slots:

    bool startProject(const Project &project);

    bool stopActivity();

    bool cancelActivity();

private:
    CommandStatus run(QStringList &args);

    Project _activeProject;
    QHash<QString, Project> _cachedProjects;

    QString _gotimePath;
    bool _bashScript;

    void cacheProjects(const QList<Project> &projects);
};

#endif //GOTIME_UI_GOTIMECONTROL_H
