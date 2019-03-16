#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>

#include "ProjectEditorDialog.h"
#include "source/gotime/ProjectStatusManager.h"

ProjectEditorDialog::ProjectEditorDialog(const Project &project, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) : QDialog(parent), Ui::ProjectDialog(),
                                                                                                                                              _control(control),
                                                                                                                                              _statusManager(statusManager),
                                                                                                                                              _project(project) {
    setupUi(this);

    loadProject(project);

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &ProjectEditorDialog::saveProject);
    connect(_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [this] { loadProject(_project); });
}

void ProjectEditorDialog::show(const Project &project, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) {
    auto *dialog = new ProjectEditorDialog(project, control, statusManager, parent);
    dialog->showNormal();
}

void ProjectEditorDialog::loadProject(const Project &project) {
    _nameEdit->setText(project.getShortName());

    // hide the edited project and all child projects in the tree
    // this selection is invalid
    _parentBox->setup(_control, _statusManager, QList<Project>() << project);
    _parentBox->setSelectedProject(project.getParentID());

    _hourlyRateEdit->setText(project.getHourlyRate());
}

void ProjectEditorDialog::saveProject() {
    const QString &id = _parentBox->selectedProject().getID();
    _control->updateProjects(QStringList() << _project.getID(), true, _nameEdit->text(), true, id, true, _hourlyRateEdit->text());
}
