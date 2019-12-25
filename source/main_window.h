#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QLabel>

#include "ui_main_window.h"
#include "GlobalShortcuts.h"
#include "data/Project.h"
#include "settings/TomSettings.h"
#include "gotime/TomControl.h"
#include "gotime/ProjectStatusManager.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
Q_OBJECT

public:
    explicit MainWindow(TomControl *control, ProjectStatusManager *statusManager, TomSettings *settings,
                        GlobalShortcuts *globalShortcuts, QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void helpAbout();

    void resetAllData();

    void selectCurrentProject(bool showWindow = false);

    void stopCurrentProject(bool restart = false);

private slots:

    void refreshData();

    void createProject();

    void importMacTimeTracker();

    void importFanurio();

    void importWatson();

    void startCurrentProject();

    void deleteCurrentProject();

    void editCurrentProject();

    void editCurrentTimeEntry();

    void deleteSelectedTimeEntries();

    void archiveSelectedTimeEntries();

    void createReport();

    void onProjectSelectionChange(const Project &);

    void onProjectStatusChange();

    void onEntrySelectionChange(const QItemSelection &);

    void lookupProject();

    void focusProjectTree();

    void focusEntriesList();

    void focusChanged(QWidget *old, QWidget *now);

    void updateStatusBar();

    void openApplicationSettings();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void writeSettings();

private:
    TomControl *_control;
    ProjectStatusManager *_statusManager;
    TomSettings *_settings;
    GlobalShortcuts *_globalShortcuts;
    QLabel *_frameStatusLabel;

    void readSettings();
};

#endif
