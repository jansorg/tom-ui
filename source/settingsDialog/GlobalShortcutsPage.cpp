#include "GlobalShortcutsPage.h"
#include "GlobalShortcutsTableModel.h"

GlobalShortcutsPage::GlobalShortcutsPage(QWidget *parent) : QWidget(parent),
                                                            Ui::GlobalShortcutsPage() {
    setupUi(this);
}

void GlobalShortcutsPage::setup(GlobalShortcuts *shortcuts) {
    _actionTable->setModel(new GlobalShortcutsTableModel(shortcuts, this));
}