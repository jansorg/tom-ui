#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>

#include "projectlookup.h"

#include "source/commonModels/projectlistmodel.h"
#include "source/gotime/StartStopProjectAction.h"
#include "source/icons.h"
#include "source/model/ProjectTreeModel.h"
#include "source/model/UserRoles.h"
#include "source/main_window.h"

ProjectLookup::ProjectLookup(TomControl *control, QMainWindow *window, QWidget *parent) : QDialog(parent),
                                                                                          _control(control) {
    setWindowFlags(Qt::Tool);
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    projectNameEdit->setup(_control);
    activeProjectLabel->setup(_control);

    connect(projectNameEdit, &ProjectCompletionLineEdit::completionAccepted, [this] {
        done(0);
    });

    if (window) {
        showMainWindowButton->setIcon(Icons::showMainWindow());
        connect(showMainWindowButton, &QPushButton::clicked, [this, window]{
            window->show();
            done(0);
        });
    } else {
        showMainWindowButton->hide();
    }

    // setup status of the active project if there's one
    stopProjectButton->setIcon(Icons::projectStop());
    connect(stopProjectButton, &QPushButton::clicked, [this] {
        _control->stopActivity();
        done(0);
    });

    const auto &active = _control->cachedStatus();
    if (!active.isValid) {
        activeProjectStatus->setEnabled(false);
    }
}

void ProjectLookup::show(TomControl *control, QMainWindow *window, QWidget *parent) {
    auto *dialog = new ProjectLookup(control, window, parent);
    dialog->showNormal();
}
