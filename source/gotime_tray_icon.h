#ifndef GOTIME_UI_TRAY_ICON_H
#define GOTIME_UI_TRAY_ICON_H


#include <QtCore/QObject>
#include <QtWidgets/QSystemTrayIcon>

#include "gotime/GotimeControl.h"
#include "gotime/StartStopProjectAction.h"

class GotimeTrayIcon : public QObject {
Q_OBJECT

public:
    GotimeTrayIcon(GotimeControl *control, QObject *parent);

private:
    void updateProjects();

    QMenu *_menu;
    GotimeControl *control;
    QSystemTrayIcon *_trayIcon;
    QList<QAction *> _projectActions;
    QAction *_separatorAction;
};

#endif
