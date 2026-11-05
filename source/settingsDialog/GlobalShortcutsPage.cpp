#include "GlobalShortcutsPage.h"
#include "GlobalShortcutsTableModel.h"

#include <QLabel>
#include <QPushButton>

GlobalShortcutsPage::GlobalShortcutsPage(QWidget *parent) : QWidget(parent),
                                                            Ui::GlobalShortcutsPage() {
    setupUi(this);
}

void GlobalShortcutsPage::setup(GlobalShortcuts *shortcuts) {
    _actionTable->setModel(new GlobalShortcutsTableModel(shortcuts, this));

    if (shortcuts->isManagedByDesktop()) {
        auto *info = new QLabel(tr("Global shortcuts are managed by your desktop environment. "
                                   "The shortcut defined here is only suggested to the desktop and may be "
                                   "changed in the system settings."), this);
        info->setWordWrap(true);
        gridLayout->addWidget(info, 1, 0);

        if (shortcuts->canConfigureInDesktop()) {
            auto *configureButton = new QPushButton(tr("Configure in System Settings…"), this);
            connect(configureButton, &QPushButton::clicked, shortcuts, &GlobalShortcuts::configureInDesktop);
            gridLayout->addWidget(configureButton, 2, 0, Qt::AlignLeft);
        }
    }
}