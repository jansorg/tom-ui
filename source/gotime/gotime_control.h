#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore>
#include <QtCore/QString>
#include <data/frame.h>

#include "data/Project.h"
#include "command_status.h"
#include "gotime_status.h"
#include "projectstatus.h"

class gotime_control : public QObject {
Q_OBJECT

public:
    explicit gotime_control(QString gotimePath, bool bashScript, QObject *parent);

    QList<Project> loadProjects();

    gotime_status status();
    const ProjectsStatus projectsStatus();

    bool isStarted(Project &project);

    QList<Frame*> loadFrames(QString projectID, bool includeSubprojects);

signals:

    void projectStarted(const Project &);

    void projectStopped(const Project &);

    void projectCancelled(const Project &);


public slots:
    bool startProject(Project &project);

    bool stopActivity();

    bool cancelActivity();

private:
    command_status run(QStringList &args);

    QString _activeProjectID;

    bool _bashScript;
    QString _gotimePath;
};

#endif //GOTIME_UI_GOTIMECONTROL_H
