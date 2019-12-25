#include "ApplicationShortcutsPage.h"
#include "ApplicationShortcutsTableModel.h"

ApplicationShortcutsPage::ApplicationShortcutsPage(QWidget *parent) : QWidget(parent),
                                                                      Ui::ApplicationShortcutsPage() {
    setupUi(this);
}

void ApplicationShortcutsPage::setup(MainWindow *window, TomSettings *settings, QList<QAction *> additionalActions) {
    QList<QAction *> actions;
    actions << additionalActions;
    for (const auto child : window->children()) {
        if (auto *action = qobject_cast<QAction *>(child)) {
            if (!action->objectName().isEmpty()) {
                actions << action;
            }
        }
    }

    auto *model = new ApplicationShortcutsTableModel(actions, settings, this);
    _actionTable->setModel(model);
}

