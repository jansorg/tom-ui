#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>

#include "ui_main_window.h"
#include "data/Project.h"
#include "gotime/gotime_control.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(gotime_control *control, QMainWindow *parent = nullptr);

    ~MainWindow() override;

private slots:

    void projectChanged(const QModelIndex &index);

    void refreshData();

private:
    Ui::MainWindow ui;
    void createActions();

    gotime_control *gotimeControl;
};

#endif
