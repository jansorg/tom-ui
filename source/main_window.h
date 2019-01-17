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
    explicit MainWindow(GotimeControl *control, QMainWindow *parent = nullptr);

    ~MainWindow() override;

private slots:

    void projectStatusChanged(const Project &project);

    void refreshData();

    void loadFrames(const Project &project);

private:
    void createActions();

    Ui::MainWindow ui;

    GotimeControl *gotimeControl;

    Project _selectedProject;
};

#endif
