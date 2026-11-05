#include "GlobalShortcuts.h"

#include <QGuiApplication>

#include "projectlookup/projectlookup.h"
#include "GlobalShortcut.h"
#include "main_window.h"

#ifdef TOM_XDG_PORTAL
#include "portal/XdgPortalGlobalShortcuts.h"
#endif

GlobalShortcuts::GlobalShortcuts(TomControl *control, QObject *parent) : QObject(parent),
                                                                         _control(control),
                                                                         _mainWindow(nullptr),
                                                                         _portal(nullptr) {
#ifdef TOM_XDG_PORTAL
    _portal = XdgPortalGlobalShortcuts::create(QGuiApplication::desktopFileName(), this);
    if (_portal != nullptr) {
        connect(_portal, &XdgPortalGlobalShortcuts::activated, this, [this](const QString &id) {
            for (auto *shortcut: _shortcuts) {
                if (shortcut->id() == id) {
                    emit shortcut->activated();
                }
            }
        });
        connect(_portal, &XdgPortalGlobalShortcuts::triggersChanged,
                this, &GlobalShortcuts::assignedTriggersChanged);
    }
#endif
}

void GlobalShortcuts::setupShortcuts(MainWindow *window) {
    _mainWindow = window;

    // project lookup action with default shortcut
    auto *lookupProject = addShortcut("global.lookupProject", tr("Lookup Project"),
                                      Qt::ShiftModifier | Qt::ControlModifier | Qt::Key_P);
    connect(lookupProject, &GlobalShortcut::activated,
            this, [this] { ProjectLookup::show(_control, _mainWindow, nullptr); });

    // start timer action
    auto *startTimer = addShortcut("global.startTimer", tr("Start timer"), QKeySequence());
    connect(startTimer, &GlobalShortcut::activated,
            this, [this] { _mainWindow->stopCurrentProject(true); });

    // stop timer action
    auto *stopTimer = addShortcut("global.stopTimer", tr("Stop timer"), QKeySequence());
    connect(stopTimer, &GlobalShortcut::activated,
            this, [this] { _mainWindow->stopCurrentProject(false); });

    // load current values
    QSettings settings;
    for (auto *shortcut: _shortcuts) {
        shortcut->load(settings);
    }

    updatePortalShortcuts();
}

QList<GlobalShortcut *> GlobalShortcuts::globalShortcuts() const {
    return _shortcuts;
}

GlobalShortcut *GlobalShortcuts::addShortcut(const QString &id, const QString &label,
                                             const QKeySequence &defaultShortcut) {
    auto *shortcut = new GlobalShortcut(id, label, defaultShortcut, _portal == nullptr, this);
    _shortcuts << shortcut;
    if (_portal != nullptr) {
        connect(shortcut, &GlobalShortcut::shortcutChanged, this, &GlobalShortcuts::updatePortalShortcuts);
    }
    return shortcut;
}

void GlobalShortcuts::updatePortalShortcuts() {
#ifdef TOM_XDG_PORTAL
    if (_portal == nullptr) {
        return;
    }

    QList<XdgPortalGlobalShortcuts::Shortcut> portalShortcuts;
    for (const auto *shortcut: _shortcuts) {
        portalShortcuts << XdgPortalGlobalShortcuts::Shortcut{shortcut->id(), shortcut->label(), shortcut->shortcut()};
    }
    _portal->setShortcuts(portalShortcuts);
#endif
}

bool GlobalShortcuts::isManagedByDesktop() const {
    return _portal != nullptr;
}

QString GlobalShortcuts::assignedTrigger(const GlobalShortcut *shortcut) const {
#ifdef TOM_XDG_PORTAL
    if (_portal != nullptr) {
        return _portal->triggerDescription(shortcut->id());
    }
#else
    Q_UNUSED(shortcut)
#endif
    return {};
}

bool GlobalShortcuts::canConfigureInDesktop() const {
#ifdef TOM_XDG_PORTAL
    return _portal != nullptr && _portal->canConfigure();
#else
    return false;
#endif
}

void GlobalShortcuts::configureInDesktop() {
#ifdef TOM_XDG_PORTAL
    if (_portal != nullptr) {
        _portal->configure();
    }
#endif
}
