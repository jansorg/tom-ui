#ifndef GOTIME_UI_TRAY_ICON_H
#define GOTIME_UI_TRAY_ICON_H


#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>

#include "gotime/GotimeControl.h"
#include "gotime/StartStopProjectAction.h"

class GotimeTrayIcon : public QObject {
Q_OBJECT

public:
    GotimeTrayIcon(GotimeControl *control, QMainWindow *mainWindow);

private slots:
    void updateStatus();

private:
    void updateProjects();

    QTimer *_timer;
    QMenu *_menu;
    GotimeControl *_control;
    QSystemTrayIcon *_trayIcon;
    QList<QAction *> _projectActions;
    QAction *_separatorAction;
};

#endif
