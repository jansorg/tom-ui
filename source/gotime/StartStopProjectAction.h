#ifndef GOTIME_UI_STARTPROJECTACTION_H
#define GOTIME_UI_STARTPROJECTACTION_H

#include <QtWidgets/QAction>

#include "data/Project.h"
#include "TomControl.h"

class StartProjectAction : public QAction {
Q_OBJECT

public:
    explicit StartProjectAction(const Project &project, TomControl *control, QObject *parent);

private slots:
    void toggleProjectStatus();

private:
    TomControl *_control;
    Project _project;
};


#endif //GOTIME_UI_STARTPROJECTACTION_H
