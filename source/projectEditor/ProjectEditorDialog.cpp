#include <QtWidgets/QDialogButtonBox>

#include "ProjectEditorDialog.h"
#include "source/gotime/ProjectStatusManager.h"

ProjectEditorDialog::ProjectEditorDialog(const Project &project, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) : QDialog(parent), Ui::ProjectDialog(),
                                                                                                                                              _control(control),
                                                                                                                                              _statusManager(statusManager),
                                                                                                                                              _project(project) {
    setupUi(this);
    loadProject(project);
}

void ProjectEditorDialog::show(const Project &project, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) {
    auto *dialog = new ProjectEditorDialog(project, control, statusManager, parent);
    dialog->showNormal();
}

void ProjectEditorDialog::loadProject(const Project &project) {
    _nameEdit->setText(project.getShortName());

    _parentBox->setup(_control, _statusManager);
    _parentBox->setSelectedProject(project.getParentID());

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &ProjectEditorDialog::saveProject);
}

void ProjectEditorDialog::saveProject() {
    const QString &id = _parentBox->selectedProject().getID();
    qDebug() << "saving project" << id;
    _control->updateProjects(QStringList() << _project.getID(), true, _nameEdit->text(), true, id);
}
