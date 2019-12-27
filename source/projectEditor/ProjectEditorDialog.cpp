#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>

#include "ProjectEditorDialog.h"
#include "gotime/ProjectStatusManager.h"

void ProjectEditorDialog::show(const Project &project, TomControl *control,
                               ProjectStatusManager *statusManager, QWidget *parent) {
    auto *dialog = new ProjectEditorDialog(project, control, statusManager, parent);
    dialog->showNormal();
}

ProjectEditorDialog::ProjectEditorDialog(const Project &project, TomControl *control,
                                         ProjectStatusManager *statusManager, QWidget *parent)
        : QDialog(parent), Ui::ProjectDialog(), _control(control), _statusManager(statusManager), _project(project) {

    setupUi(this);
    setModal(true);
    setWindowTitle(tr("Edit Project %1").arg(project.getShortName()));

    _noteRequired->initItems(tr("Inherit from parent"), tr("Required"), tr("Not required"));

    // fixme handle root project
    Project parentProject = _control->cachedProject(project.getParentID());
    if (parentProject.isValid()) {
        _noteRequired->setInheritedValue(parentProject.appliedIsNoteRequired() ? tr("Required") : tr("Not required"));
    }

    loadProject(project);

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &ProjectEditorDialog::saveProject);
    connect(_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [this] { loadProject(_project); });
}

void ProjectEditorDialog::loadProject(const Project &project) {
    _nameEdit->setText(project.getShortName());

    // hide the edited project and all child projects in the tree
    // this selection is invalid
    _parentBox->setup(_control, _statusManager, QList<Project>() << project);
    _parentBox->setSelectedProject(project.getParentID());

    _hourlyRateEdit->setText(project.getHourlyRate());

    _noteRequired->setState(project.isNoteRequired());
}

void ProjectEditorDialog::saveProject() {
    const QString &parentId = _parentBox->selectedProject().getID();
    _control->updateProjects(QStringList() << _project.getID(),
                             true, _nameEdit->text().simplified(),
                             true, parentId,
                             true, _hourlyRateEdit->text(),
                             true, _noteRequired->getState());
}
