//
// Created by jansorg on 19.01.19.
//

#include <QtCore>
#include <QtWidgets/QInputDialog>

#include "commondialogs.h"

Project CommonDialogs::createProject(const Project &parentProject, GotimeControl *control, QWidget *parent) {
    bool ok;
    QString projectName = QInputDialog::getText(parent, QObject::tr("Create Project"), QObject::tr("Project name:"), QLineEdit::Normal, "", &ok);
    if (ok && !projectName.isEmpty()) {
        return control->createProject(parentProject.getID(), projectName);
    }
    return Project();
}
