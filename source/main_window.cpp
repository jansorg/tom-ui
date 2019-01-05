#include "model/ProjectTreeModel.h"
#include "gotime/GotimeControl.h"
#include "gotime/StartStopProjectAction.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl * control, QMainWindow *parent) : gotimeControl(control), QMainWindow(parent) {
    ui.setupUi(this);
    createActions();

    auto *model = new ProjectTreeModel(control, this);
    ui.projectTree->setModel(model);

//    const GotimeStatus status = gotimeControl->status();
//    if (status.isValid) {
//        qDebug() << "Active project: " << status.currentProject()->getName() << status.startTime().toString(Qt::SystemLocaleShortDate);
//    } else {
//        qDebug() << "No active project found";
//    }
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
