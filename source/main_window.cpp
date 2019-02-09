#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>
#include <dialogs/CommonDialogs.h>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <source/projectEditor/ProjectEditorDialog.h>

#include "version.h"
#include "icons.h"
#include "main_window.h"
#include "ActionUtils.h"

#include "source/report/ProjectReportDialog.h"
#include "source/projectlookup/projectlookup.h"

MainWindow::MainWindow(TomControl *control, ProjectStatusManager *statusManager, QMainWindow *parent) : QMainWindow(parent),
                                                                                                        Ui::MainWindow(),
                                                                                                        _control(control),
                                                                                                        _statusManager(statusManager) {
//#ifndef Q_OS_MAC
    setWindowIcon(Icons::LogoLarge());
//#endif

    setupUi(this);
#ifndef TOM_REPORTS
    menuReports->setEnabled(false);
    menuReports->setVisible(false);
#endif

    _projectTree->setup(control, statusManager);
    _frameView->setup(control);

    actionRefresh->setIcon(Icons::refreshData());
    actionResetAllData->setIcon(Icons::resetData());
    actionMinimize->setIcon(Icons::windowHide());
    actionQuit->setIcon(Icons::exit());

    actionProjectLookup->setIcon(Icons::projectLookup());
    actionProjectStart->setIcon(Icons::projectStart());
    actionProjectStop->setIcon(Icons::projectStop());
    actionProjectCreate->setIcon(Icons::projectNew());
    actionProjectEdit->setIcon(Icons::projectEdit());
    actionProjectRemove->setIcon(Icons::projectRemove());
    actionProjectSelectActive->setIcon(Icons::projectSelectActive());

    actionTimeEntryRemove->setIcon(Icons::timeEntryDelete());
    actionTimeEntryArchive->setIcon(Icons::timeEntryArchive());

    actionReportCreate->setIcon(Icons::report());

    actionWindowFocusProjects->setIcon(Icons::windowProjectsFocus());
    actionWindowFocusProjects->setIcon(Icons::windowTimeEntriesFocus());

    actionImportMacTimeTracker->setIcon(Icons::importTableData());
    actionImportFanurio->setIcon(Icons::importData());
    actionImportWatson->setIcon(Icons::importData());

    actionHelpAbout->setIcon(Icons::about());

    connect(_projectTree, &ProjectTreeView::projectSelected, _frameView, &FrameTableView::onProjectSelected);
    connect(_projectTree, &ProjectTreeView::projectSelected, this, &MainWindow::onProjectSelectionChange);

    connect(_frameView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onEntrySelectionChange);

    connect(actionSettingsShowArchived, &QAction::triggered, _frameView, &FrameTableView::setShowArchived);
    connect(actionSettingsShowArchived, &QAction::triggered, _projectTree, &ProjectTreeView::setShowArchived);
    connect(actionSettingsShowArchived, &QAction::triggered, _statusManager, &ProjectStatusManager::setIncludeArchived);

    connect(_control, &TomControl::projectStarted, this, &MainWindow::onProjectStatusChange);
    connect(_control, &TomControl::projectStopped, this, &MainWindow::onProjectStatusChange);

    connect(actionQuit, &QAction::triggered, &QCoreApplication::quit);

    createActions();
    refreshData();
}

void MainWindow::refreshData() {
    _projectTree->refresh();
    onProjectStatusChange();
    onEntrySelectionChange(QItemSelection());
}

MainWindow::~MainWindow() = default;


void MainWindow::createActions() {
}

void MainWindow::helpAbout() {
    QString about = QString(
            "Tom is a simple UI for the <a href=\"https://github.com/jansorg/tom-ui\">tom time tracker</a> command line application.<br><br>Version: %1")
            .arg(PROJECT_VERSION);

    QMessageBox::about(this, "About Tom", about);
}

void MainWindow::createProject() {
    CommonDialogs::createProject(Project(), _control, this);
}

void MainWindow::importMacTimeTracker() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Mac Time Tracker CSV export"),
                                                    "",
                                                    tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importMacTimeTracker(fileName);
    }
}

void MainWindow::importFanurio() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Fanurio CSV export"),
                                                    "",
                                                    tr("CSV Files (*.csv)"));

    if (!fileName.isEmpty()) {
        _control->importFanurioCSV(fileName);
    }
}

void MainWindow::importWatson() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Watson frames file"),
                                                    "",
                                                    tr("Watson frames files (frames)"));

    if (!fileName.isEmpty()) {
        _control->importWatsonFrames(fileName);
    }
}

void MainWindow::resetAllData() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Reset data"),
                                                              tr("Do you want to remove all projects, tags and frames?"),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        _control->resetAll();
    }
}

void MainWindow::onProjectSelectionChange(const Project &current) {
    bool valid = current.isValid();

    actionProjectStart->setEnabled(valid);
    actionProjectRemove->setEnabled(valid);
    actionProjectEdit->setEnabled(valid);
}

void MainWindow::onProjectStatusChange() {
    bool hasActiveProject = _control->cachedStatus().currentProject().isValid();

    actionProjectStop->setEnabled(hasActiveProject);
    actionProjectSelectActive->setEnabled(hasActiveProject);
}

void MainWindow::startCurrentProject() {
    const Project &project = _projectTree->getCurrentProject();
    if (project.isValid()) {
        _control->startProject(project);
    }
}

void MainWindow::stopCurrentProject() {
    _control->stopActivity();
}

void MainWindow::deleteSelectedTimeEntries() {
    _frameView->deleteSelectedEntries();
}

void MainWindow::archiveSelectedTimeEntries() {
    _frameView->archiveSelectedEntries();
}

void MainWindow::deleteCurrentProject() {
    ActionUtils::removeProject(_control, _projectTree->getCurrentProject(), this);
}

void MainWindow::selectCurrentProject() {
    _projectTree->setFocus();
    _projectTree->selectProject(_control->cachedActiveProject());
}

void MainWindow::editCurrentProject() {
    const Project &project = _projectTree->getCurrentProject();
    if (project.isValid()) {
        ProjectEditorDialog::show(project, _control, _statusManager, this);
    }
}

void MainWindow::onEntrySelectionChange(const QItemSelection &selection) {
    bool selectedEntries = !selection.isEmpty();
    actionTimeEntryRemove->setEnabled(selectedEntries);
    actionTimeEntryArchive->setEnabled(selectedEntries);
}

void MainWindow::createReport() {
    QList<Project> selected;
    const Project &project = _projectTree->getCurrentProject();
    if (project.isValid()) {
        selected << project;
    }
    
    auto *dialog = new ProjectReportDialog(selected, _control, _statusManager, this);
    dialog->show();
}

void MainWindow::lookupProject() {
    ProjectLookup::show(_control, this, this);
}

void MainWindow::focusProjectTree() {
    _projectTree->setFocus();
}

void MainWindow::focusEntriesList() {
    _frameView->setFocus();
}

