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

    void projectChanged(const QModelIndex &index);

    void refreshData();

private:
    Ui::MainWindow ui;
    void createActions();

    GotimeControl *gotimeControl;
};

#endif
