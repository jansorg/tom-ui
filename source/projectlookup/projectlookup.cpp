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
    activeProjectLabel->setup(_control);

    connect(projectNameEdit, &ProjectCompletionLineEdit::completionAccepted, [this] {
        qDebug() << "done!";
        done(0);
    });

    // setup status of the active project if there's one
    stopProjectButton->setIcon(Icons::projectStop());
    connect(stopProjectButton, &QPushButton::clicked, [this] {
        _control->stopActivity();
        close();
    });

    const auto &active = _control->cachedStatus();
    if (!active.isValid) {
        activeProjectStatus->setEnabled(false);
    }
}

void ProjectLookup::show(TomControl *control, QWidget *parent) {
    auto *dialog = new ProjectLookup(control, parent);
    dialog->showNormal();
}
