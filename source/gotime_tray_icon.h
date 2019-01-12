#ifndef GOTIME_UI_TRAY_ICON_H
#define GOTIME_UI_TRAY_ICON_H


#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>

#include "gotime/gotime_control.h"
#include "gotime/startStop_project_action.h"

class GotimeTrayIcon : public QObject {
Q_OBJECT

public:
    GotimeTrayIcon(gotime_control *control, QMainWindow *mainWindow);

private slots:

    void updateStatus();

private slots:
    void projectStarted(const Project &);
    void projectStopped(const Project &);
    void updateIcon();

private:
    void updateProjects();

    QTimer *_iconTimer;
    QTimer *_statusUpdateTimer;

    QMenu *_menu;
    gotime_control *_control;
    QSystemTrayIcon *_trayIcon;
    QList<QAction *> _projectActions;
    QAction *_separatorAction;

    int _activeIconIndex;
    QVector<QPixmap> _activeIcons;
    QPixmap _stoppedIcon;

    void loadIcons();
};

#endif
