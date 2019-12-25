#include <QtWidgets/QTreeView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainWindow>
#include <dialogs/CommonDialogs.h>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <source/frameEditor/FrameEditorDialog.h>

#include "settingsDialog/SettingsDialog.h"
#include "version.h"
#include "icons.h"
#include "main_window.h"
#include "ActionUtils.h"
#include "projectEditor/ProjectEditorDialog.h"
#include "report/ProjectReportDialog.h"
#include "projectlookup/projectlookup.h"

MainWindow::MainWindow(TomControl *control,
                       ProjectStatusManager *statusManager,
                       TomSettings *settings,
                       GlobalShortcuts *globalShortcuts,
                       QWidget *parent)
        : QMainWindow(parent),
          Ui::MainWindow(),
          _control(control),
          _statusManager(statusManager),
          _settings(settings),
          _globalShortcuts(globalShortcuts),
          _frameStatusLabel(new QLabel(this)) {
//#ifndef Q_OS_MAC
    setWindowIcon(Icons::LogoLarge());
//#endif

    setupUi(this);
#ifndef TOM_REPORTS
    menuReports->setEnabled(false);
    menuReports->setVisible(false);
#endif

    _projectTree->setup(control, statusManager);
    _frameView->setup(control, statusManager);

    // setup the statusbar
    mainStatusBar->addPermanentWidget(_frameStatusLabel, 1);
    _frameStatusLabel->setAlignment(Qt::AlignRight);

    connect(_projectTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::updateStatusBar);
    connect(_projectTree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateStatusBar);
    connect(_frameView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateStatusBar);
    connect(_frameView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::updateStatusBar);

    // fix icons
    actionRefresh->setIcon(Icons::refreshData());
    actionResetAllData->setIcon(Icons::resetData());
    actionMinimize->setIcon(Icons::windowHide());
    actionQuit->setIcon(Icons::exit());

    actionProjectLookup->setIcon(Icons::projectLookup());
    actionProjectStart->setIcon(Icons::projectStart());
    actionProjectStop->setIcon(Icons::stopTimer());
    actionProjectCreate->setIcon(Icons::projectNew());
    actionProjectEdit->setIcon(Icons::projectEdit());
    actionProjectSelectActive->setIcon(Icons::projectSelectActive());

    actionTimeEntryEdit->setIcon(Icons::frameEdit());

    actionReportCreate->setIcon(Icons::report());

    actionWindowFocusProjects->setIcon(Icons::windowProjectsFocus());
    actionWindowFocusProjects->setIcon(Icons::windowTimeEntriesFocus());

    actionImportMacTimeTracker->setIcon(Icons::importTableData());
    actionImportFanurio->setIcon(Icons::importData());
    actionImportWatson->setIcon(Icons::importData());

    actionHelpAbout->setIcon(Icons::about());

    // fix shortcut context to be only active when the parent widget contains focus

    // add missing connections
    connect(_projectTree, &ProjectTreeView::projectSelected, _frameView, &FrameTableView::onProjectSelected);
    connect(_projectTree, &ProjectTreeView::projectSelected, this, &MainWindow::onProjectSelectionChange);

    connect(_frameView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onEntrySelectionChange);

    connect(actionSettingsShowArchived, &QAction::toggled, _projectTree, &ProjectTreeView::setShowArchived);

    connect(actionProjectsTodayColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowTodayColumn);
    connect(actionProjectsYesterdayColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowYesterdayColumn);
    connect(actionProjectsWeekColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowWeekColumn);
    connect(actionProjectsMonthColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowMonthColumn);
    connect(actionProjectsYearColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowYearColumn);
    connect(actionProjectsTotalColumn, &QAction::toggled, _projectTree, &ProjectTreeView::setShowTotalColumn);

    connect(actionSettingsShowArchived, &QAction::toggled, _frameView, &FrameTableView::setShowArchived);
    connect(actionSettingsShowArchived, &QAction::toggled, _statusManager, &ProjectStatusManager::setIncludeArchived);

    connect(actionTimeEntryLastUpdatedColumn, &QAction::toggled, _frameView, &FrameTableView::setShowLastUpdatedColumn);

    connect(_control, &TomControl::projectStatusChanged, this, &MainWindow::onProjectStatusChange);

    connect(actionQuit, &QAction::triggered, &QCoreApplication::quit);

    connect(QGuiApplication::instance(), &QCoreApplication::aboutToQuit, this, &MainWindow::writeSettings);

    // listen to focus events
    connect(dynamic_cast<QApplication *>(QCoreApplication::instance()), &QApplication::focusChanged,
            this, &MainWindow::focusChanged);

    //fix up menus
    menuSettings->insertSection(actionProjectsTodayColumn, tr("Projects"));
    menuSettings->insertSection(actionSettingsShowArchived, tr("Time Entries"));

    // setup our custom actions
    menuProject->addAction(_projectTree->getDeleteAction());
    menuEntries->addAction(_frameView->getDeleteAction());

    refreshData();

    readSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::writeSettings() {
    _projectTree->writeSettings();
    _frameView->writeSettings();

    QSettings settings;
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("mainwindow/splitterState", splitter->saveState());

    for (const auto child : children()) {
        if (auto *action = qobject_cast<QAction *>(child)) {
            _settings->saveAction(action);
        }
    }
}

void MainWindow::readSettings() {
    _projectTree->readSettings();
    _frameView->readSettings();

    QSettings settings;
    QVariant geometry = settings.value("mainwindow/geometry");
    if (geometry.isValid()) {
        restoreGeometry(geometry.toByteArray());
    }

    QVariant splitterState = settings.value("mainwindow/splitterState");
    if (splitterState.isValid()) {
        splitter->restoreState(splitterState.toByteArray());
    }

    for (const auto child: children()) {
        if (auto *action = qobject_cast<QAction *>(child)) {
            _settings->loadAction(action);
        }
    }
}

void MainWindow::refreshData() {
    _projectTree->refresh();
    onProjectStatusChange();
    onProjectSelectionChange(Project());
    onEntrySelectionChange(QItemSelection());
}

MainWindow::~MainWindow() = default;


void MainWindow::helpAbout() {
    QString about = QString(
            "Tom is a simple UI for the <a href=\"https://github.com/jansorg/tom-ui\">tom time tracker</a> command line application.<br><br>Version: %1")
            .arg(PROJECT_VERSION);

    QMessageBox::about(this, "About Tom", about);
}

void MainWindow::createProject() {
    Project newProject = CommonDialogs::createProject(Project(), _control, this);
    if (newProject.isValid()) {
        _projectTree->selectProject(newProject);
    }
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
        // fixme notes for currently active time entry
        _control->startProject(project);
    }
}

void MainWindow::stopCurrentProject(bool restart) {
    const auto currentProject = _control->cachedActiveProject();
    const Project &project = _control->cachedProject(currentProject.getID());
    bool noteRequired = currentProject.isValid() && project.appliedIsNoteRequired();

    QString notes;
    if (noteRequired) {
        //fixme pass current notes?
        bool ok;
        notes = QInputDialog::getMultiLineText(
                this,
                tr("Enter required notes"),
                tr("Project: <strong>%1</strong><br>Notes are required to stop the current time entry.")
                        .arg(project.getName()),
                "", &ok);
        if (!ok) {
            // don't continue to stop the current ask when the dialog was cancelled
            return;
        }
    }

    if (_control->stopActivity(notes) && restart) {
        _control->startProject(project);
    }
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

void MainWindow::selectCurrentProject(bool showWindow) {
    qDebug() << "selectCurrentProject";

    if (showWindow) {
        if (!isVisible()) {
            ensurePolished();
            setWindowState(windowState() & ~Qt::WindowMinimized);
            show();
        }
        raise();
        activateWindow();
    }

    _projectTree->setFocus();
    _projectTree->selectProject(_control->cachedActiveProject());
}

void MainWindow::editCurrentProject() {
    const Project &project = _projectTree->getCurrentProject();
    if (project.isValid()) {
        ProjectEditorDialog::show(project, _control, _statusManager, this);
    }
}

void MainWindow::editCurrentTimeEntry() {
    QList<Frame *> frames = _frameView->selectedFrames();
    if (frames.size() == 1) {
        Frame *frame = frames[0];
        FrameEditorDialog::show(*frame, _control, _statusManager, this);
    }
}

void MainWindow::onEntrySelectionChange(const QItemSelection &selection) {
    int count = selection.size();
    actionTimeEntryArchive->setEnabled(count >= 1);
    actionTimeEntryEdit->setEnabled(count == 1);
}

void MainWindow::createReport() {
    QList<Project> selected;
    const Project &project = _projectTree->getCurrentProject();
    if (project.isValid()) {
        selected << project;
    }

    auto *dialog = new ProjectReportDialog(selected, _control, _statusManager, this);
    dialog->exec();
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

void MainWindow::focusChanged(QWidget *, QWidget *now) {
    if (now == _projectTree) {
        if (!_projectTree->hasSelectedProject()) {
            _projectTree->selectFirstRow();
        }

        _projectTree->getDeleteAction()->setEnabled(true);
        _frameView->getDeleteAction()->setEnabled(false);
        actionTimeEntryEdit->setEnabled(false);

        actionProjectEdit->setEnabled(_projectTree->getCurrentProject().isValid());
        actionTimeEntryArchive->setEnabled(false);
    } else if (now == _frameView) {
        if (!_frameView->hasSelectedFrames()) {
            _frameView->selectFirstFrame();
        }

        _projectTree->getDeleteAction()->setEnabled(false);
        _frameView->getDeleteAction()->setEnabled(true);

        actionTimeEntryEdit->setEnabled(_frameView->selectedFrames().size() == 1);
        actionTimeEntryArchive->setEnabled(_frameView->hasSelectedFrames());

        actionProjectEdit->setEnabled(false);
    }
}

void MainWindow::updateStatusBar() {
    auto selected = _frameView->selectedFrames();
    if (selected.size() > 1) {
        qlonglong millis = 0;
        for (auto f : selected) {
            millis += f->durationMillis(true);
        }
        Timespan span(millis);
        _frameStatusLabel->setText(tr("Total: %1 / %2", "statusbar").arg(span.format()).arg(span.formatDecimal()));
    } else {
        _frameStatusLabel->setText("");
    }
}

void MainWindow::openApplicationSettings() {
    QList<QAction *> additionalActions;
    additionalActions << _frameView->getDeleteAction();
    additionalActions << _projectTree->getDeleteAction();

    auto *dialog = new SettingsDialog(this, _globalShortcuts, _settings, additionalActions);
    dialog->open();
}
