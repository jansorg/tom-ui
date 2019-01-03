#include "main_window.h"
#include "moc_main_window.cpp"
#include "Project.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    createActions();
    createTrayIcon();

    trayIcon->show();
}

MainWindow::~MainWindow() {
    delete this->projectActions;
}

void MainWindow::createActions() {
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);


    this->projectActions = new QList<QAction*>();
}

//void MainWindow::updatePopupActions() {
//    for (auto action : *this->projectActions) {
//        this->trayIconMenu->removeAction(action);
//    }
//
//}

void MainWindow::createTrayIcon() {
    const QIcon &icon = QIcon(":/images/trayicon.png");

    trayIconMenu = new QMenu(this);
    trayIconMenu->addSection(tr("Projects"));
    QAction *kite = new QAction("Kite", this);
    QAction *tezos= new QAction("Tezos", this);
    QAction *jetbrains= new QAction("JetBrains", this);
    trayIconMenu->addAction(kite);
    trayIconMenu->addAction(tezos);
    trayIconMenu->addAction(jetbrains);

    trayIconMenu->addSeparator()->setText("Actions");
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(QString("Today: <b>7h</b>"));
}

