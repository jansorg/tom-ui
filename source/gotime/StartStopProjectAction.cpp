#include <icons.h>
#include "StartStopProjectAction.h"

StartProjectAction::StartProjectAction(const Project &project, TomControl *control, QObject *parent) : QAction(parent),
                                                                                                          _control(control),
                                                                                                          _project(project) {

    setIconVisibleInMenu(true);
    setText(project.getName());
    setIcon(control->isStarted(project) ? Icons::stopTimer() : Icons::startTimer());

    connect(this, SIGNAL(triggered()), this, SLOT(toggleProjectStatus()));
}

void StartProjectAction::toggleProjectStatus() {
    if (_control->isStarted(_project)) {
        _control->stopActivity();
    } else {
        _control->startProject(_project);
    }
}
