#include "StartProjectAction.h"

StartProjectAction::StartProjectAction(Project *project, QObject *parent) : QAction(parent) {
    this->m_project = project;
    this->setText("Start: " + project->getName());
    this->setIcon(QIcon(":/images/start.svg"));

    connect(this, SIGNAL(triggered()),
            this, SLOT(slotForwardStartProject())
    );
}

void StartProjectAction::slotForwardStartProject() {
    emit projectTriggered(m_project);
}

