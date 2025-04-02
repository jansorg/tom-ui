#ifndef GOTIME_UI_TRAY_ICON_H
#define GOTIME_UI_TRAY_ICON_H


#include <QtCore/QObject>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include "gotime/TomControl.h"
#include "gotime/StartStopProjectAction.h"
#include "main_window.h"

class GotimeTrayIcon : public QObject {
Q_OBJECT

public:
    GotimeTrayIcon(TomControl *control, MainWindow *mainWindow);

    bool eventFilter(QObject *watched, QEvent *event) override;

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
