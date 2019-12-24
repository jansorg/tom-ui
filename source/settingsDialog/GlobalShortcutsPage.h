#ifndef TOM_UI_GLOBALSHORTCUTSPAGE_H
#define TOM_UI_GLOBALSHORTCUTSPAGE_H

#include "ui_GlobalShortcutsPage.h"
#include "GlobalShortcuts.h"

class GlobalShortcutsPage : public QWidget, private Ui::GlobalShortcutsPage {
Q_OBJECT

public:
    explicit GlobalShortcutsPage(QWidget *parent = nullptr);

    void setup(GlobalShortcuts *shortcuts);

private slots:
    void shortcutSelected(int row);
    void keySequenceChanged(const QKeySequence& keySequence);
    void clearKeySequence();

private:
    void registerGlobalShortcuts();

    GlobalShortcuts *_shortcuts;
    QList<GlobalShortcut *> _shortcutsList;
    QList<QTableWidgetItem *> _shortcutItems;
};


#endif //TOM_UI_GLOBALSHORTCUTSPAGE_H
