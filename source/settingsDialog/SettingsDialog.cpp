#include "SettingsDialog.h"

#include "ui_ApplicationSettingsPage.h"
#include "ui_ApplicationShortcutsPage.h"
#include "ui_GlobalShortcutsPage.h"

SettingsDialog::SettingsDialog(MainWindow *window,
                               GlobalShortcuts *globalShortcuts,
                               TomSettings *settings,
                               QList<QAction *> additionalActions)
        : QDialog(), Ui::SettingsDialog() {

    setupUi(this);

    applicationSettings->setup(settings);
    keyboardSettings->setup(window, additionalActions);
    globalKeyboardSettings->setup(globalShortcuts);

    pagesList->setCurrentRow(0, QItemSelectionModel::SelectionFlag::ClearAndSelect);
}
