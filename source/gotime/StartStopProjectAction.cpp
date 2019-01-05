#include "StartStopProjectAction.h"

const Project &StartProjectAction::getProject() {
    return _project;
}

StartProjectAction::StartProjectAction(Project &project, GotimeControl *control, QObject *parent) : _control(control),
                                                                                                    _project(project),
                                                                                                    QAction(parent) {
    if (control->isStarted(project)) {
        projectStarted(project);
    } else {
        projectStopped(project);
    }

    connect(this, SIGNAL(triggered()),
            this, SLOT(toggleProjectStatus()));

    connect(control, SIGNAL(projectStarted(const Project &)),
            this, SLOT(projectStarted(const Project &)));
    connect(control, SIGNAL(projectStopped(const Project &)),
            this, SLOT(projectStopped(const Project &)));
//    connect(control, SIGNAL(projectCancelled(const Project &)),
//            this, SLOT(projectStopped(const Project &)));
}

void StartProjectAction::projectStarted(const Project &project) {
    if (project.getID() == _project.getID()) {
        this->setText("Stop: " + project.getName());
        this->setIcon(QIcon(":/images/stop.svg"));
    }
}

void StartProjectAction::projectStopped(const Project &project) {
    if (project.getID() == _project.getID()) {
        this->setText("Start: " + project.getName());
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
