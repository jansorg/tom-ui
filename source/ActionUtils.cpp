#include <QtWidgets/QMessageBox>

#include "ActionUtils.h"

bool ActionUtils::removeProject(TomControl *control, const Project &project, QWidget *parent) {
    if (!project.isValid()) {
        return false;
    }

    auto button = QMessageBox::question(parent,
                                        QWidget::tr("Delete project"),
                                        QString("Do you want to delete project <em>%1</em> and all subprojects and attached time entries?").arg(
                                                project.getName()));
    if (button == QMessageBox::Yes) {
        return control->removeProject(project);
    }
    return false;
}
