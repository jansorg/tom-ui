#include <model/frametableviewmodel.h>
#include "model/ProjectTreeModel.h"
#include "gotime/StartStopProjectAction.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl *control, QMainWindow *parent) : gotimeControl(control), QMainWindow(parent) {
    ui.setupUi(this);
    createActions();

    auto *model = new ProjectTreeModel(control, this);
    ui.projectTree->setModel(model);

    connect(ui.projectTree, &QTreeView::activated, this, &MainWindow::projectChanged);
}

MainWindow::~MainWindow() {
    delete this->projectActions;
}

void MainWindow::projectChanged(const QModelIndex &index) {
    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    if (item && item->getProject().isValid()) {
        auto frames = gotimeControl->loadFrames(item->getProject().getID());
        auto *frameModel = new FrameTableViewModel(frames, this);
        ui.frameView->setModel(frameModel);
    }
}

void MainWindow::createActions() {
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &QCoreApplication::quit);


    this->projectActions = new QList<QAction *>();
}
