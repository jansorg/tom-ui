#include "model/ProjectTreeModel.h"
#include "gotime/GotimeControl.h"
#include "gotime/StartProjectAction.h"
#include "main_window.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    gotimeControl = new GotimeControl("/home/jansorg/bin/gotime", this);

    createActions();
    createTrayIcon();

    trayIcon->show();

    auto projects = gotimeControl->loadProjects();
    auto *model = new ProjectTreeModel(projects, this);
    ui.projectTree->setModel(model);
}

MainWindow::~MainWindow() {
    delete this->projectActions;
}

void MainWindow::createActions() {
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);


    this->projectActions = new QList<QAction *>();
}

void MainWindow::startProject(Project *project) {
    gotimeControl->start(project);
}

void MainWindow::cancelProject() {
    gotimeControl->cancel();
}

void MainWindow::stopProject() {
    gotimeControl->stop();
}

void MainWindow::createTrayIcon() {
    const QIcon &icon = QIcon(":/images/trayicon.png");

    trayIconMenu = new QMenu(this);
    trayIconMenu->addSection(tr("Projects"));

    const QList<Project> projects = gotimeControl->loadProjects();
    for (auto project : projects) {
        auto action = new StartProjectAction(&project, this);
        trayIconMenu->addAction(action);
        connect(action, SIGNAL(projectTriggered(Project * )),
                this, SLOT(startProject(Project * )));
    }

    QAction *stopAction = new QAction("Stop timer...");
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopProject()));

    QAction *cancelAction = new QAction("Cancel timer...");
    connect(cancelAction, SIGNAL(triggered()), this, SLOT(cancelProject()));

    trayIconMenu->addSeparator()->setText("Actions");
    trayIconMenu->addAction(stopAction);
    trayIconMenu->addAction(cancelAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Today: <b>8:00 hours</b><br>This week: <b>32:00 hours</b>");
}

