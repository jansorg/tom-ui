#include <QtGui>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMainWindow>

#include "gotime/TomControl.h"
#include "gotime/StartStopProjectAction.h"

#include "tray.h"
#include "icons.h"
#include "osenvinfo.h"

GotimeTrayIcon::GotimeTrayIcon(TomControl *control, QMainWindow *mainWindow) : QObject(),
                                                                               _control(control),
                                                                               _trayIcon(new QSystemTrayIcon(this)) {

    loadIcons();

    _menu = new QMenu();

    _stopTaskAction = new QAction("Stop active project...");
    _stopTaskAction->setToolTip("Stop the current project and record data.");
    _stopTaskAction->setIconVisibleInMenu(true);
    connect(_stopTaskAction, SIGNAL(triggered()), control, SLOT(stopActivity()));
    _menu->addAction(_stopTaskAction);

    _menu->addSection(tr("Projects"));

    _separatorAction = _menu->addSeparator();
    _separatorAction->setText("Actions");

    QAction *showWindowAction = new QAction(Icons::showMainWindow(), "&Show window", this);
    showWindowAction->setIconVisibleInMenu(true);
    connect(showWindowAction, &QAction::triggered, mainWindow, &QMainWindow::show);
    _menu->addAction(showWindowAction);

    QAction *quitAction = new QAction(Icons::exit(), "&Quit", this);
    connect(quitAction, &QAction::triggered, &QCoreApplication::quit);
    _menu->addAction(quitAction);

    _trayIcon->setContextMenu(_menu);
    _trayIcon->setIcon(_stoppedIcon);
    _trayIcon->show();

    _statusUpdateTimer = new QTimer(this);
    connect(_statusUpdateTimer, &QTimer::timeout, this, &GotimeTrayIcon::updateStatus);

    connect(control, &TomControl::projectStarted, this, &GotimeTrayIcon::updateStatus);
    connect(control, &TomControl::projectStopped, this, &GotimeTrayIcon::updateStatus);
    connect(control, &TomControl::projectCancelled, this, &GotimeTrayIcon::updateStatus);
    connect(control, &TomControl::projectUpdated, this, &GotimeTrayIcon::updateStatus);

    updateStatus();
}

void GotimeTrayIcon::updateProjects() {
    // remove existing actions
    for (auto action : _projectActions) {
        _menu->removeAction(action);
    }
    _projectActions.clear();

    for (const auto &project : _control->loadRecentProjects()) {
        _projectActions << new StartProjectAction(project, _control, this);
    }
    _menu->insertActions(_separatorAction, _projectActions);
}

void GotimeTrayIcon::updateStatus() {
    _lastStatus = _control->cachedStatus();

    updateProjects();

    if (_lastStatus.isValid) {
        const Timespan span = Timespan::of(_lastStatus.startTime(), QDateTime::currentDateTime());
        QString tooltip;
        if (OSEnvInfo::supportsHTMLTooltips()) {
            tooltip = QString("%1: <b>%2</b>").arg(_lastStatus.currentProject().getName()).arg(span.format());
        } else {
            tooltip = QString("%1: %2").arg(_lastStatus.currentProject().getName()).arg(span.format());
        }
        _trayIcon->setToolTip(tooltip);

        _trayIcon->setIcon(_startedIcon);

        _stopTaskAction->setEnabled(true);
        _statusUpdateTimer->start(5000);
    } else {
        _trayIcon->setToolTip("No active project");
        _trayIcon->setIcon(_stoppedIcon);
        _stopTaskAction->setEnabled(false);
        _statusUpdateTimer->stop();
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
