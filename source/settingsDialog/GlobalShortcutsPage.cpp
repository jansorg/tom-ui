#include "GlobalShortcutsPage.h"

GlobalShortcutsPage::GlobalShortcutsPage(QWidget *parent) : QWidget(parent),
                                                            Ui::GlobalShortcutsPage(),
                                                            _shortcuts(nullptr) {
    setupUi(this);
}

void GlobalShortcutsPage::setup(GlobalShortcuts *shortcuts) {
    _shortcuts = shortcuts;

    registerGlobalShortcuts();

    connect(actionsTable, &QTableWidget::currentCellChanged,
            this, &GlobalShortcutsPage::shortcutSelected);
    connect(keySequenceEdit, &QKeySequenceEdit::keySequenceChanged,
            this, &GlobalShortcutsPage::keySequenceChanged);
    connect(clearKeySequenceButton, &QToolButton::clicked,
            this, &GlobalShortcutsPage::clearKeySequence);
}

void GlobalShortcutsPage::registerGlobalShortcuts() {
    _shortcutsList = _shortcuts->globalShortcuts();

    actionsTable->setRowCount(_shortcutsList.size());

    for (int i = 0; i < _shortcutsList.size(); i++) {
        const GlobalShortcut *shortcut = _shortcutsList[i];
        actionsTable->setItem(i, 0, new QTableWidgetItem(shortcut->label()));

        auto shortcutItem = new QTableWidgetItem(shortcut->shortcut().toString());
        actionsTable->setItem(i, 1, shortcutItem);
        _shortcutItems << shortcutItem;
    }
}

void GlobalShortcutsPage::shortcutSelected(int row) {
    auto *shortcut = _shortcutsList[row];
    if (shortcut != nullptr) {
        keySequenceEdit->setKeySequence(shortcut->shortcut());
    }
}

void GlobalShortcutsPage::keySequenceChanged(const QKeySequence &keySequence) {
    auto row = actionsTable->currentRow();
    auto *shortcut = _shortcutsList[row];
    if (shortcut != nullptr) {
        shortcut->setShortcut(keySequence);
        _shortcutItems[row]->setText(keySequence.toString());

        QSettings settings;
        shortcut->save(settings);
    }
}

void GlobalShortcutsPage::clearKeySequence() {
    keySequenceChanged(QKeySequence());
    keySequenceEdit->clear();
}
