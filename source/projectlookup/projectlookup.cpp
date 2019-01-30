#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>

#include "source/gotime/StartStopProjectAction.h"
#include "source/icons.h"
#include "source/model/ProjectTreeModel.h"
#include "source/model/UserRoles.h"
#include "projectlookup.h"
#include "source/commonModels/projectlistmodel.h"

ProjectLookup::ProjectLookup(TomControl *control, QWidget *parent) : QDialog(parent), _control(control) {
    setWindowFlags(Qt::Tool);
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    projectNameEdit->setup(_control);
    connect(projectNameEdit, &ProjectCompletionLineEdit::completionAccepted, [this] { done(0); });

    activeProjectLabel->setup(_control);

    // setup status of the active project if there's one
    stopProjectButton->setIcon(Icons::projectStop());
    const auto &active = _control->cachedStatus();
    if (active.isValid) {
        connect(stopProjectButton, &QPushButton::clicked, [this] {
            _control->stopActivity();
            close();
        });
    } else {
        activeProjectStatus->setEnabled(false);
    }
}


void ProjectLookup::show(TomControl *control, QWidget *parent) {
    auto *dialog = new ProjectLookup(control, parent);
    dialog->showNormal();
}

void ProjectLookup::onProjectStart(const QModelIndex &index) {
    const QString &id = index.data(UserRoles::IDRole).toString();
    const QString &name = index.data(Qt::DisplayRole).toString();
    qDebug() << "project to start" << name << id;

    bool ok = _control->startProject(_control->cachedProject(id));
    if (ok) {
        close();
    } else {
        QMessageBox::warning(parentWidget(), "Error", QString("Failed to start project %1.").arg(name));
        projectNameEdit->clear();
        projectNameEdit->setFocus();
    }
}
