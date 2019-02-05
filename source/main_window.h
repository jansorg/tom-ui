#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>
#include <gotime/ProjectStatusManager.h>

#include "ui_main_window.h"
#include "data/Project.h"
#include "gotime/TomControl.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
Q_OBJECT

public:
    explicit MainWindow(TomControl *control, ProjectStatusManager *statusManager, QMainWindow *parent = nullptr);

    ~MainWindow() override;

public slots:

    void helpAbout();

    void resetAllData();

private slots:

    void refreshData();

    void createProject();

    void importMacTimeTracker();

    void importFanurio();

    void importWatson();

    void startCurrentProject();

    void stopCurrentProject();

    void deleteCurrentProject();

    void selectCurrentProject();

    void deleteSelectedTimeEntries();

    void createReport();

    void onProjectSelectionChange(const Project &);

    void onProjectStatusChange();

    void onEntrySelectionChange(const QItemSelection &);

    void lookupProject();

    void focusProjectTree();

    void focusEntriesList();

private:
    void createActions();

    TomControl *_control;
    ProjectStatusManager *_statusManager;
};

#endif
