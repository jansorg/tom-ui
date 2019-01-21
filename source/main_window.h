#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>
#include <gotime/projectstatusmanager.h>

#include "ui_main_window.h"
#include "data/Project.h"
#include "gotime/GotimeControl.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(GotimeControl *control, ProjectStatusManager *statusManager, QMainWindow *parent = nullptr);

    ~MainWindow() override;

public slots:

    void helpAbout();
    void resetAllData();

private slots:

    void refreshData();

    void createProject();

    void importMacTimeTracker();

    void importFanurio();

private:
    void createActions();

    Ui::MainWindow ui;
    GotimeControl *_control;
};

#endif
