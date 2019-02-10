#ifndef GOTIME_UI_TRAY_ICON_H
#define GOTIME_UI_TRAY_ICON_H


#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>

#include "gotime/TomControl.h"
#include "gotime/StartStopProjectAction.h"

class GotimeTrayIcon : public QObject {
Q_OBJECT

public:
    GotimeTrayIcon(TomControl *control, QMainWindow *mainWindow);

private slots:

    void updateAll();

    void updateIconAndTooltip();

private:
    void updateContextMenu();

    QMenu *_menu;
    TomControl *_control;
    QSystemTrayIcon *_trayIcon;
    QList<QAction *> _projectActions;
    QAction *_stopTaskAction;
    QAction *_separatorAction;

    QIcon _startedIcon;
    QIcon _stoppedIcon;

    void loadIcons();

};

#endif
