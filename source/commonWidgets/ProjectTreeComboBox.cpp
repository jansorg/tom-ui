#include "ProjectTreeComboBox.h"

#include <QtCore>
#include <QMouseEvent>
#include <QHeaderView>
#include <QAbstractItemView>

#include "view/ProjectTreeView.h"

ProjectTreeComboBox::ProjectTreeComboBox(QWidget *parent) : QComboBox(parent),
                                                            _skipNextHide(false),
                                                            _control(nullptr),
                                                            _sourceModel(nullptr),
                                                            _sortModel(nullptr) {
}

void ProjectTreeComboBox::setup(TomControl *control, ProjectStatusManager *statusManager, const QList<Project> &hiddenProjects) {
    setFocusPolicy(Qt::StrongFocus);

    _control = control;
    _sourceModel = new ProjectTreeModel(control, statusManager, true, this, false, false);
    _sourceModel->loadProjects();
    for (const auto &p: hiddenProjects) {
        _sourceModel->removeProject(p);
    }

    _sortModel = new QSortFilterProxyModel(this);
    _sortModel->setSourceModel(_sourceModel);
    _sortModel->sort(ProjectTreeItem::COL_NAME, Qt::AscendingOrder);
    setModel(_sortModel);

    setModelColumn(ProjectTreeItem::COL_NAME);

    _view = new ProjectTreeView(this);
    setView(_view);

    for (int col = ProjectTreeItem::FIRST_STATUS_COL_INDEX; col <= ProjectTreeItem::LAST_COL_INDEX; col++) {
        _view->hideColumn(col);
    }
    _view->setDragEnabled(false);
    _view->setAcceptDrops(false);
    _view->expandToDepth(1);

    _view->header()->hide();
    _view->viewport()->installEventFilter(this);
}

void ProjectTreeComboBox::hidePopup() {
    if (_skipNextHide) {
        _skipNextHide = false;
    } else {
        QComboBox::hidePopup();
    }
}

bool ProjectTreeComboBox::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress && watched == view()->viewport()) {
        qDebug() << "event filter";
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        QModelIndex index = view()->indexAt(mouseEvent->pos());
        if (!view()->visualRect(index).contains(mouseEvent->pos())) {
            _skipNextHide = true;
        } else {
            emit indexSelected(index);
        }
    }
    return QComboBox::eventFilter(watched, event);
}

void ProjectTreeComboBox::setSelectedProject(const Project &project) {
    setSelectedProject(project.getID());
}

void ProjectTreeComboBox::setSelectedProject(const QString &id) {
    if (id.isEmpty()) {
        _view->setCurrentIndex(QModelIndex());
    } else {
        const QModelIndex &sourceIndex = _sourceModel->getProjectRow(id);
        qDebug() << "setting project to" << sourceIndex;

        // hack to preselec the item in a QComboBox which is only row based (without a parent index)
        const QModelIndex &newCurrentIndex = _sortModel->mapFromSource(sourceIndex);
        _view->setCurrentIndex(newCurrentIndex);
        setRootModelIndex(newCurrentIndex.parent());
        setCurrentIndex(newCurrentIndex.row());
        setRootModelIndex(QModelIndex());
    }
}

Project ProjectTreeComboBox::selectedProject() {
    return _sourceModel->projectAtIndex(_sortModel->mapToSource(_view->currentIndex()));
}
