#ifndef GOTIME_UI_STARTPROJECTACTION_H
#define GOTIME_UI_STARTPROJECTACTION_H

#include <QtWidgets/QAction>

#include "data/Project.h"
#include "gotime_control.h"

class StartProjectAction : public QAction {
Q_OBJECT

public:
    explicit StartProjectAction(Project &project, gotime_control *control, QObject *parent);

    const Project &getProject();

private slots:
    void projectStarted(const Project &project);
    void projectStopped(const Project &project);

private slots:
    void toggleProjectStatus();

private:
    gotime_control *_control;
    Project _project;
};


#endif //GOTIME_UI_STARTPROJECTACTION_H
