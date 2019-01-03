#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>
#include "ui_main_window.h"

/**
 * Main window class for the application
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = nullptr);

    virtual ~MainWindow();

private:
    Ui::MainWindow ui;

    void createActions();
    void createTrayIcon();

    void updatePopupActions();

    QAction* minimizeAction;
    QAction* quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QList<QAction*> *projectActions;
};

#endif
