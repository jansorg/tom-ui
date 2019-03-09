#include <QtGui>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMainWindow>

#include "gotime/TomControl.h"
#include "gotime/StartStopProjectAction.h"

#include "tray.h"
#include "icons.h"
#include "osenvinfo.h"
#include "main_window.h"

GotimeTrayIcon::GotimeTrayIcon(TomControl *control, MainWindow *mainWindow) : QObject(),
                                                                               _control(control),
                                                                               _trayIcon(new QSystemTrayIcon(this)) {

    loadIcons();

    _menu = new QMenu();

    _stopTaskAction = new QAction(tr("Stop timer"));
    _stopTaskAction->setToolTip(tr("Stop the current project and record data."));
    _stopTaskAction->setIcon(Icons::stopTimer());
    _stopTaskAction->setIconVisibleInMenu(true);
    connect(_stopTaskAction, SIGNAL(triggered()), control, SLOT(stopActivity()));
    _menu->addAction(_stopTaskAction);

    _menu->addSection(tr("Projects"));

    _separatorAction = _menu->addSeparator();
    _separatorAction->setText("Actions");

    QAction *showWindowAction = new QAction(Icons::showMainWindow(), tr("&Show window"), this);
    connect(showWindowAction, &QAction::triggered, mainWindow, [mainWindow]{
        mainWindow->selectCurrentProject(true);
    });
    _menu->addAction(showWindowAction);

    QAction *quitAction = new QAction(Icons::exit(), tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, &QCoreApplication::quit);
    _menu->addAction(quitAction);

    _trayIcon->setContextMenu(_menu);
    _trayIcon->setIcon(_stoppedIcon);
    _trayIcon->show();

    connect(control, &TomControl::projectStatusChanged, this, &GotimeTrayIcon::updateAll);
    connect(control, &TomControl::projectUpdated, this, &GotimeTrayIcon::updateAll);

    connect(_trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            _control->stopActivity();
        }
    });

    updateAll();

    // update tooltip every 30s
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GotimeTrayIcon::updateIconAndTooltip);
    timer->start(30000);
}

void GotimeTrayIcon::updateAll() {
    updateContextMenu();
    updateIconAndTooltip();
}

void GotimeTrayIcon::updateContextMenu() {
    // remove existing actions
    for (auto action : _projectActions) {
        _menu->removeAction(action);
    }
    _projectActions.clear();

    bool useCheckmarks = false;
#ifdef Q_OS_MAC
    useCheckmarks = true;
#endif

    for (const auto &project : _control->cachedRecentProjects()) {
        _projectActions << new StartProjectAction(project, _control, this, useCheckmarks);
    }
    _menu->insertActions(_separatorAction, _projectActions);
}

void GotimeTrayIcon::updateIconAndTooltip() {
    auto lastStatus = _control->cachedStatus();

    if (lastStatus.isValid) {
        const Timespan span = Timespan::of(lastStatus.startTime(), QDateTime::currentDateTime());
        QString tooltip;
        if (OSEnvInfo::supportsHTMLTooltips()) {
            tooltip = QString(tr("%1: <b>%2</b>")).arg(lastStatus.currentProject().getName()).arg(span.format());
        } else {
            tooltip = QString(tr("%1: %2")).arg(lastStatus.currentProject().getName()).arg(span.format());
        }
        _trayIcon->setToolTip(tooltip);
        _trayIcon->setIcon(_startedIcon);

        _stopTaskAction->setEnabled(true);
    } else {
        _trayIcon->setToolTip("No active project");
        _trayIcon->setIcon(_stoppedIcon);
        _stopTaskAction->setEnabled(false);
    }
}

void GotimeTrayIcon::loadIcons() {
#ifdef Q_OS_MAC
    _startedIcon = QIcon(QPixmap(":/icons/osx/trayicon-started.svg"));
    _stoppedIcon = QIcon(QPixmap(":/icons/osx/trayicon-stopped.svg"));
    _stoppedIcon.setIsMask(true);
#else
    _startedIcon = QIcon(QPixmap(":/icons/trayicon-started.svg"));
    _stoppedIcon = QIcon(QPixmap(":/icons/trayicon-stopped.svg"));
#endif
}
