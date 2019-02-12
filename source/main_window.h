#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>
#include <source/settings/TomSettings.h>

#include "ui_main_window.h"
#include "data/Project.h"
#include "gotime/TomControl.h"
#include "gotime/ProjectStatusManager.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
Q_OBJECT

public:
    explicit MainWindow(TomControl *control, ProjectStatusManager *statusManager, TomSettings *settings, QMainWindow *parent = nullptr);

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

    void editCurrentProject();

    void deleteSelectedTimeEntries();

    void archiveSelectedTimeEntries();

    void createReport();

    void onProjectSelectionChange(const Project &);

    void onProjectStatusChange();

    void onEntrySelectionChange(const QItemSelection &);

    void lookupProject();

    void focusProjectTree();

    void focusEntriesList();

protected:
    void closeEvent(QCloseEvent *event) override;

private:

    TomControl *_control;
    ProjectStatusManager *_statusManager;
    TomSettings *_settings;

    void createActions();

    void readSettings();

    void writeSettings();
};

#endif
