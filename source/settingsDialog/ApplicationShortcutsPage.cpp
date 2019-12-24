#include "ApplicationShortcutsPage.h"

ApplicationShortcutsPage::ApplicationShortcutsPage(QWidget *parent) : QWidget(parent),
                                                                      Ui::ApplicationShortcutsPage(),
                                                                      _settings(nullptr) {
    setupUi(this);
}

void ApplicationShortcutsPage::setup(MainWindow *window, QList<QAction *> additionalActions) {
    connect(actionsTable, &QTableWidget::currentCellChanged,
            this, &ApplicationShortcutsPage::shortcutSelected);
    connect(keySequenceEdit, &QKeySequenceEdit::keySequenceChanged,
            this, &ApplicationShortcutsPage::keySequenceChanged);
    connect(clearKeySequenceButton, &QToolButton::clicked,
            this, &ApplicationShortcutsPage::clearKeySequence);

    QList<QAction *> actions;
    actions << additionalActions;
    for (const auto child : window->children()) {
        if (auto *action = qobject_cast<QAction *>(child)) {
            if (!action->objectName().isEmpty()) {
                actions << action;
            }
        }
    }

    actionsTable->setRowCount(actions.size());

    for (auto *action : actions) {
        registerAction(action);
    }
}

void ApplicationShortcutsPage::registerAction(QAction *action) {
    auto *shortcutItem = new QTableWidgetItem(action->shortcut().toString());
    _actionItems << shortcutItem;
    _actions << action;

    int row = _actions.size() - 1;

    // fixme use a better label for actions where its meaning is not clear?
    QString label = action->text();
    actionsTable->setItem(row, 0, new QTableWidgetItem(label.remove("&")));
    actionsTable->setItem(row, 1, shortcutItem);
}

void ApplicationShortcutsPage::shortcutSelected(int row) {
    keySequenceEdit->setKeySequence(_actions[row]->shortcut());
}

void ApplicationShortcutsPage::keySequenceChanged(const QKeySequence &keySequence) {
    auto row = actionsTable->currentRow();
    auto *action = _actions[row];
    if (action != nullptr) {
        action->setShortcut(keySequence);
        _actionItems[row]->setText(keySequence.toString());

        _settings->saveAction(action);
    }
}

void ApplicationShortcutsPage::clearKeySequence() {
    keySequenceChanged(QKeySequence());
    keySequenceEdit->clear();
}
