#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>

#include "ui_main_window.h"
#include "data/Project.h"
#include "gotime/GotimeControl.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = nullptr);

    ~MainWindow() override;

public slots:

    void startProject(Project *project);

    void cancelProject();

    void stopProject();

private:
    Ui::MainWindow ui;

    void createActions();

    void createTrayIcon();

//    void updatePopupActions();

    QAction *minimizeAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QList<QAction *> *projectActions;
    GotimeControl *gotimeControl;
};

#endif
