#include <QtGui>
#include <QtWidgets/QMenu>

#include <gotime/GotimeControl.h>
#include <gotime/startStop_project_action.h>
#include <QtWidgets/QMainWindow>
#include "gotime_tray_icon.h"

GotimeTrayIcon::GotimeTrayIcon(GotimeControl *control, QMainWindow *mainWindow) : _control(control),
                                                                                  _trayIcon(new QSystemTrayIcon(this)),
                                                                                  QObject() {

    loadIcons();

    _menu = new QMenu();

    _stopTaskAction = new QAction("Stop active project...");
    _stopTaskAction->setToolTip("Stop the current project and record data.");
    connect(_stopTaskAction, SIGNAL(triggered()), control, SLOT(stopActivity()));
    _menu->addAction(_stopTaskAction);

    _cancelTaskAction = new QAction("Cancel active project...");
    _cancelTaskAction->setToolTip("Stop the active project without recording new data.");
    connect(_cancelTaskAction, SIGNAL(triggered()), control, SLOT(cancelActivity()));
    _menu->addAction(_cancelTaskAction);

    _menu->addSection(tr("Projects"));

    _separatorAction = _menu->addSeparator();
    _separatorAction->setText("Actions");
    QAction *showWindowAction = new QAction("&Show window", this);
    connect(showWindowAction, &QAction::triggered, mainWindow, &QMainWindow::show);
    _menu->addAction(showWindowAction);

    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);
    _menu->addAction(quitAction);

    _trayIcon->setContextMenu(_menu);
    _trayIcon->setIcon(_stoppedIcon);
    _trayIcon->show();

    updateProjects();

    _statusUpdateTimer = new QTimer(this);
    connect(_statusUpdateTimer, &QTimer::timeout, this, &GotimeTrayIcon::updateStatus);

    _iconTimer = new QTimer(this);
    connect(_iconTimer, &QTimer::timeout, this, &GotimeTrayIcon::updateIcon);
    connect(control, &GotimeControl::projectStarted, this, &GotimeTrayIcon::projectStarted);
    connect(control, &GotimeControl::projectStopped, this, &GotimeTrayIcon::projectStopped);
    connect(control, &GotimeControl::projectCancelled, this, &GotimeTrayIcon::projectStopped);
    connect(control, &GotimeControl::projectUpdated, this, &GotimeTrayIcon::projectUpdated);

    const GotimeStatus &status = control->status();
    if (status.isValid) {
        const Project &project = status.currentProject();
        if (project.isValid()) {
            _trayIcon->setIcon(_activeIcons.at(0));

            projectStarted(project);
        } else {
            projectStopped(project);
        }
    }
}

void GotimeTrayIcon::updateProjects() {
    //remove existing actions
    for (auto action : _projectActions) {
        _menu->removeAction(action);
    }
    _projectActions.clear();

    for (auto project : _control->loadRecentProjects(6)) {
        auto action = new StartProjectAction(project, _control, this);
        _projectActions << action;
    }
    _menu->insertActions(_separatorAction, _projectActions);
}

void GotimeTrayIcon::updateStatus() {
    const GotimeStatus &status = _control->status();
    if (status.isValid) {
        const Timespan span = Timespan::of(status.startTime(), QDateTime::currentDateTime());

        _trayIcon->setToolTip(QString("%1: <b>%2</b>").arg(status.currentProject().getName()).arg(span.format()));
    } else {
        _trayIcon->setToolTip("No active project");
    }
}

void GotimeTrayIcon::loadIcons() {
    _stoppedIcon = QPixmap(":/images/trayicon-stopped.svg");
    _activeIcons << QPixmap(":/images/trayicon-1.svg");
    _activeIcons << QPixmap(":/images/trayicon-2.svg");
    _activeIcons << QPixmap(":/images/trayicon-3.svg");
    _activeIcons << QPixmap(":/images/trayicon-4.svg");
}

void GotimeTrayIcon::projectStarted(const Project & project) {
    if (!_iconTimer->isActive()) {
        _trayIcon->setIcon(_activeIcons.at(0));

        _activeIconIndex = 0;
        _iconTimer->start(1000);
    }

    if (!_statusUpdateTimer->isActive()) {
        _statusUpdateTimer->start(2500);
    }

    _stopTaskAction->setEnabled(true);
    _cancelTaskAction->setEnabled(true);

    projectUpdated(project);
}

void GotimeTrayIcon::projectStopped(const Project & project ) {
    _trayIcon->setIcon(_stoppedIcon);

    _iconTimer->stop();
    _statusUpdateTimer->stop();

    _stopTaskAction->setEnabled(false);
    _cancelTaskAction->setEnabled(false);

    projectUpdated(project);
}

void GotimeTrayIcon::projectUpdated(const Project &) {
    updateStatus();
    updateProjects();
}

void GotimeTrayIcon::updateIcon() {
    _activeIconIndex = _activeIconIndex % _activeIcons.size();
    _trayIcon->setIcon(_activeIcons.at(_activeIconIndex));
    _activeIconIndex++;
}
