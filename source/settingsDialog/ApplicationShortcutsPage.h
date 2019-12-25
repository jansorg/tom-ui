#ifndef TOM_UI_APPLICATIONSHORTCUTSPAGE_H
#define TOM_UI_APPLICATIONSHORTCUTSPAGE_H

#include "ui_ApplicationShortcutsPage.h"
#include "main_window.h"

class ApplicationShortcutsPage : public QWidget, private Ui::ApplicationShortcutsPage {
Q_OBJECT
public:
    explicit ApplicationShortcutsPage(QWidget *parent = nullptr);

    void setup(MainWindow *window, TomSettings* settings, QList<QAction *> additionalActions);

private:
    TomSettings *_settings;
};

#endif //TOM_UI_APPLICATIONSHORTCUTSPAGE_H
