#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>

#include "projectlookup.h"
#include "commonModels/projectlistmodel.h"
#include "icons.h"
#include "model/UserRoles.h"
#include "main_window.h"

void ProjectLookup::show(TomControl *control, MainWindow *window, QWidget *parent) {
    auto *dialog = new ProjectLookup(control, window, parent);
    dialog->exec();
}

ProjectLookup::ProjectLookup(TomControl *control, MainWindow *window, QWidget *parent)
        : QDialog(parent), _control(control) {

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
        connect(showMainWindowButton, &QPushButton::clicked, [this, window] {
            // fixme handle virtual desktops on Linux?
            window->show();
            done(0);
        });
    } else {
        showMainWindowButton->hide();
    }

    // setup status of the active project if there's one
    stopProjectButton->setIcon(Icons::stopTimer());
    connect(stopProjectButton, &QPushButton::clicked, [this, window] {
        window->stopCurrentProject();
        done(0);
    });

    const auto &active = _control->cachedStatus();
    if (!active.isValid) {
        activeProjectStatus->setEnabled(false);
    }

    readSettings();
}

void ProjectLookup::done(int i) {
    writeSettings();
    QDialog::done(i);
}

void ProjectLookup::readSettings() {
    QSettings settings;
    const QVariant &geometry = settings.value("projectLookup/geometry");
    if (geometry.isValid()) {
        restoreGeometry(geometry.toByteArray());
    }
}

void ProjectLookup::writeSettings() {
    QSettings settings;
    settings.setValue("projectLookup/geometry", saveGeometry());
}
