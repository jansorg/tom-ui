#include <icons.h>
#include "StartStopProjectAction.h"

StartProjectAction::StartProjectAction(const Project &project, TomControl *control, QObject *parent, bool checkable) : QAction(parent),
                                                                                                                       _control(control),
                                                                                                                       _project(project) {

    setCheckable(checkable);
    setIconVisibleInMenu(true);
    setText(project.getName());

    if (checkable) {
        setChecked(control->isStarted(project));
    } else {
        setIcon(control->isStarted(project) ? Icons::stopTimer() : Icons::startTimer());
    }

    connect(this, SIGNAL(triggered()), this, SLOT(toggleProjectStatus()));
}

void StartProjectAction::toggleProjectStatus() {
    if (_control->isStarted(_project)) {
        _control->stopActivity();
    } else {
        _control->startProject(_project);
    }
}
