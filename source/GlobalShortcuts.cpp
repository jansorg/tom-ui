#include "GlobalShortcuts.h"

#include "projectlookup/projectlookup.h"
#include "GlobalShortcut.h"

GlobalShortcuts::GlobalShortcuts(TomControl *control, QObject *parent) : QObject(parent),
                                                                         _control(control),
                                                                         _mainWindow(nullptr) {
}

void GlobalShortcuts::setupShortcuts(MainWindow *window) {
    _mainWindow = window;

    // project lookup action with default shortcut
    auto *lookupProject = new GlobalShortcut("global.lookupProject", tr("Lookup Project"),
                                             Qt::ShiftModifier + Qt::ControlModifier + Qt::Key_P, window);
    _shortcuts << lookupProject;
    QObject::connect(lookupProject, &GlobalShortcut::activated,
                     lookupProject, [&] { ProjectLookup::show(_control, _mainWindow, nullptr); });

    // start timer action
    auto *startTimer = new GlobalShortcut("global.startTimer", tr("Start timer"), QKeySequence(), window);
    _shortcuts << startTimer;
    QObject::connect(startTimer, &GlobalShortcut::activated,
                     startTimer, [&] { _mainWindow->stopCurrentProject(true); });

    // stop timer action
    auto *stopTimer = new GlobalShortcut("global.stopTimer", tr("Stop timer"), QKeySequence(), window);
    _shortcuts << stopTimer;
    QObject::connect(stopTimer, &GlobalShortcut::activated,
                     stopTimer, [&] { _mainWindow->stopCurrentProject(false); });

    // load current values
    QSettings settings;
    for (auto *shortcut: _shortcuts) {
        shortcut->load(settings);
    }
}

QList<GlobalShortcut *> GlobalShortcuts::globalShortcuts() const {
    return _shortcuts;
}


