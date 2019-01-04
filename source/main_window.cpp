#include <model/ProjectTreeModel.h>
#include <appControl/GotimeControl.h>
#include <appControl/StartProjectAction.h>
#include "main_window.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    createActions();
    createTrayIcon();

    trayIcon->show();

    QList<Project *> projects = QList<Project *>();
    projects << new Project("Kite", "kite-id");
    projects << new Project("Tezos", "tezos");
    projects << new Project("Iviews", "iv");

    auto *model = new ProjectTreeModel(projects, this);
    ui.projectTree->setModel(model);

    gotimeControl = new GotimeControl("/home/jansorg/bin/gotime", this);
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
    gotimeControl->cancelWork();
}

void MainWindow::stopProject() {
    gotimeControl->stopWork();
}

void MainWindow::createTrayIcon() {
    const QIcon &icon = QIcon(":/images/trayicon.png");

    trayIconMenu = new QMenu(this);
    trayIconMenu->addSection(tr("Projects"));

    QList<StartProjectAction *> actions;
    actions << new StartProjectAction(new Project("Kite", "Kite"), this);
    actions << new StartProjectAction(new Project("Intern/gotime", "Intern/gotime"), this);

    for (auto &action : actions) {
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

