#include <QInputDialog>

#include "CommonDialogs.h"

Project CommonDialogs::createProject(const Project &parentProject, TomControl *control, QWidget *parent) {
    bool ok;
    QString projectName = QInputDialog::getText(parent, QObject::tr("Create Project"), QObject::tr("Project name:"), QLineEdit::Normal, "", &ok);
    if (ok && !projectName.isEmpty()) {
        return control->createProject(parentProject.getID(), projectName.simplified());
    }
    return Project();
}
