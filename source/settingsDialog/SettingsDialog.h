#ifndef TOM_UI_SETTINGSDIALOG_H
#define TOM_UI_SETTINGSDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_settingsDialog.h"
#include "settings/TomSettings.h"

class SettingsDialog : public QDialog, private Ui::SettingsDialog {
Q_OBJECT
public:
    explicit SettingsDialog(MainWindow *window,
                            GlobalShortcuts *globalShortcuts,
                            TomSettings *settings,
                            QList<QAction*> additionalActions);
};

#endif //TOM_UI_SETTINGSDIALOG_H
