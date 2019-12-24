#ifndef TOM_UI_APPLICATIONSHORTCUTSPAGE_H
#define TOM_UI_APPLICATIONSHORTCUTSPAGE_H

#include "ui_ApplicationShortcutsPage.h"
#include "main_window.h"

class ApplicationShortcutsPage : public QWidget, private Ui::ApplicationShortcutsPage {
Q_OBJECT
public:
    explicit ApplicationShortcutsPage(QWidget *parent = nullptr);

    void setup(MainWindow *window, QList<QAction*> additionalActions);

private slots:

    void shortcutSelected(int row);

    void keySequenceChanged(const QKeySequence &keySequence);

    void clearKeySequence();

private:
    void registerAction(QAction *action);

    TomSettings *_settings;
    QList<QAction *> _actions;
    QList<QTableWidgetItem *> _actionItems;
};


#endif //TOM_UI_APPLICATIONSHORTCUTSPAGE_H
