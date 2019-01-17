#include "startStop_project_action.h"

const Project &StartProjectAction::getProject() {
    return _project;
}

StartProjectAction::StartProjectAction(const Project &project, GotimeControl *control, QObject *parent) : QAction(parent),
                                                                                                          _control(control),
                                                                                                          _project(project) {
    if (control->isStarted(project)) {
        projectStarted(project);
    } else {
        projectStopped(project);
    }

    connect(this, SIGNAL(triggered()),
            this, SLOT(toggleProjectStatus()));

    connect(control, SIGNAL(projectStarted(
                                    const Project &)),
            this, SLOT(projectStarted(
                               const Project &)));

    connect(control, SIGNAL(projectStopped(
                                    const Project &)),
            this, SLOT(projectStopped(
                               const Project &)));
}

void StartProjectAction::projectStarted(const Project &project) {
    if (project.getID() == _project.getID()) {
        this->setText(project.getName());
        this->setIcon(QIcon(":/images/stop.svg"));
    }
}

void StartProjectAction::projectStopped(const Project &project) {
    if (project.getID() == _project.getID()) {
        this->setText(project.getName());
        this->setIcon(QIcon(":/images/start.svg"));
    }
}

void StartProjectAction::toggleProjectStatus() {
    if (_control->isStarted(_project)) {
        _control->stopActivity();
    } else {
        _control->startProject(_project);
    }
}
