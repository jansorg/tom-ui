#include <model/frametableviewmodel.h>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QHeaderView>
#include "model/ProjectTreeModel.h"
#include "gotime/startStop_project_action.h"
#include "main_window.h"

MainWindow::MainWindow(GotimeControl *control, QMainWindow *parent) : gotimeControl(control), QMainWindow(parent) {
    ui.setupUi(this);
    ui.projectTree->setControl(control);

    createActions();

    refreshData();
    connect(ui.projectTree, &QTreeView::activated, this, &MainWindow::projectChanged);
}

void MainWindow::refreshData() {
    //fixme delete old model?
    auto *model = new ProjectTreeModel(gotimeControl, this);

    QTreeView *tree = ui.projectTree;
    tree->setModel(model);
    tree->sortByColumn(0, Qt::AscendingOrder);
    tree->header()->setStretchLastSection(false);
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setCascadingSectionResizes(true);
}

MainWindow::~MainWindow() = default;

void MainWindow::projectChanged(const QModelIndex &index) {
    auto *item = static_cast<ProjectTreeItem *>(index.internalPointer());
    if (item && item->getProject().isValid()) {
        const Project &project = item->getProject();
        auto frames = gotimeControl->loadFrames(project.getID(), true);

        auto *sortedModel = new QSortFilterProxyModel(this);
        auto *frameModel = new FrameTableViewModel(frames, gotimeControl, this);
        sortedModel->setSourceModel(frameModel);
        ui.frameView->setModel(sortedModel);
        ui.frameView->sortByColumn(0, Qt::DescendingOrder);
    }
}

void MainWindow::createActions() {
}
