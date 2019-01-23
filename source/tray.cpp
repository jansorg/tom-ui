#include <QtGui>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMainWindow>

#include "gotime/GotimeControl.h"
#include "gotime/startStop_project_action.h"

#include "tray.h"
#include "icons.h"
#include "osenvinfo.h"

GotimeTrayIcon::GotimeTrayIcon(GotimeControl *control, QMainWindow *mainWindow) : QObject(),
                                                                                  _control(control),
                                                                                  _trayIcon(new QSystemTrayIcon(this)) {

    loadIcons();

    _menu = new QMenu();

    _stopTaskAction = new QAction("Stop active project...");
    _stopTaskAction->setToolTip("Stop the current project and record data.");
    connect(_stopTaskAction, SIGNAL(triggered()), control, SLOT(stopActivity()));
    _menu->addAction(_stopTaskAction);
//
    _menu->addSection(tr("Projects"));

    _separatorAction = _menu->addSeparator();
    _separatorAction->setText("Actions");
    QAction *showWindowAction = new QAction("&Show window", this);
    connect(showWindowAction, &QAction::triggered, mainWindow, &QMainWindow::show);
    _menu->addAction(showWindowAction);

    QAction *quitAction = new QAction(Icons::exit(), "&Quit", this);
    connect(quitAction, &QAction::triggered, &QCoreApplication::quit);
    _menu->addAction(quitAction);

    _trayIcon->setContextMenu(_menu);
    _trayIcon->setIcon(_stoppedIcon);
    _trayIcon->show();

    updateProjects();

    _statusUpdateTimer = new QTimer(this);
    connect(_statusUpdateTimer, &QTimer::timeout, this, &GotimeTrayIcon::updateStatus);

    connect(control, &GotimeControl::projectStarted, this, &GotimeTrayIcon::projectStarted);
    connect(control, &GotimeControl::projectStopped, this, &GotimeTrayIcon::projectStopped);
    connect(control, &GotimeControl::projectCancelled, this, &GotimeTrayIcon::projectStopped);
    connect(control, &GotimeControl::projectUpdated, this, &GotimeTrayIcon::projectUpdated);

    const GotimeStatus &status = control->status();
    _stopTaskAction->setEnabled(status.isValid);
    if (status.isValid) {
        const Project &project = status.currentProject();
        if (project.isValid()) {
            projectStarted(project);
        } else {
            projectStopped(project);
        }
    }
}

void GotimeTrayIcon::updateProjects() {
    // remove existing actions
    for (auto action : _projectActions) {
        _menu->removeAction(action);
    }
    _projectActions.clear();

    // add new entries
    for (const auto &project : _control->loadRecentProjects(6)) {
        auto action = new StartProjectAction(project, _control, this);
        _projectActions << action;
    }
    _menu->insertActions(_separatorAction, _projectActions);
}

void GotimeTrayIcon::updateStatus() {
    if (_lastStatus.isValid) {
        const Timespan span = Timespan::of(_lastStatus.startTime(), QDateTime::currentDateTime());

        QString tooltip;
        if (OSEnvInfo::supportsHTMLTooltips()) {
            tooltip = QString("%1: <b>%2</b>").arg(_lastStatus.currentProject().getName()).arg(span.format());
        } else {
            tooltip = QString("%1: %2").arg(_lastStatus.currentProject().getName()).arg(span.format());
        }
        _trayIcon->setToolTip(tooltip);
    } else {
        _trayIcon->setToolTip("No active project");
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

void GotimeTrayIcon::projectStarted(const Project &project) {
    _trayIcon->setIcon(_startedIcon);

    _lastStatus = _control->status();
    if (!_statusUpdateTimer->isActive()) {
        _statusUpdateTimer->start(2500);
    }

    _stopTaskAction->setEnabled(true);

    projectUpdated(project);
}

void GotimeTrayIcon::projectStopped(const Project &project) {
    _lastStatus = GotimeStatus();

    _trayIcon->setIcon(_stoppedIcon);
    _statusUpdateTimer->stop();
    _stopTaskAction->setEnabled(false);

    projectUpdated(project);
}

void GotimeTrayIcon::projectUpdated(const Project &) {
    updateStatus();
    updateProjects();
}