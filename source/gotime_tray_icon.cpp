#include <QtGui>
#include <QtWidgets/QMenu>

#include <gotime/GotimeControl.h>
#include <gotime/StartStopProjectAction.h>
#include "gotime_tray_icon.h"

GotimeTrayIcon::GotimeTrayIcon(GotimeControl *control, QObject *parent) : control(control),
                                                                          _trayIcon(new QSystemTrayIcon(this)),
                                                                          QObject(parent) {

    _menu = new QMenu();
    _menu->addSection(tr("Projects"));

    _separatorAction = _menu->addSeparator();
    _separatorAction->setText("Actions");

    QAction *stopAction = new QAction("Stop timer...");
    connect(stopAction, SIGNAL(triggered()), control, SLOT(stopActivity()));
    _menu->addAction(stopAction);

    QAction *cancelAction = new QAction("Cancel timer...");
    connect(cancelAction, SIGNAL(triggered()), control, SLOT(cancelActivity()));
    _menu->addAction(cancelAction);

    _menu->addSeparator();
    QAction *quitAction = new QAction("&Quit", this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);
    _menu->addAction(quitAction);

    _trayIcon->setContextMenu(_menu);
    _trayIcon->setIcon(QPixmap(":/images/trayicon.svg"));
    _trayIcon->show();

    updateProjects();
}

void GotimeTrayIcon::updateProjects() {
    //remove existing actions
    for (auto action : _projectActions) {
        _menu->removeAction(action);
    }
    _projectActions.clear();

    for (auto project : control->loadProjects()) {
        auto action = new StartProjectAction(project, control, this);
        _projectActions << action;
    }
    _menu->insertActions(_separatorAction, _projectActions);
}
