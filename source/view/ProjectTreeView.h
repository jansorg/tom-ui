#ifndef TOM_UI_PROJECTTREEVIEW_H
#define TOM_UI_PROJECTTREEVIEW_H

#include <QAction>
#include <QTreeView>

#include "model/ProjectTreeItem.h"
#include "model/ProjectTreeModel.h"
#include "gotime/TomControl.h"

class ProjectTreeView : public QTreeView {
Q_OBJECT

signals:

    void projectSelected(const Project &project);

public:
    explicit ProjectTreeView(QWidget *parent);

    void setup(TomControl *control, ProjectStatusManager *statusManager);

    Project getCurrentProject();

    void writeSettings();

    void readSettings();

    void setShowYesterdayColumn(bool show);

    void setShowTodayColumn(bool show);

    void setShowWeekColumn(bool show);

    void setShowMonthColumn(bool show);

    void setShowYearColumn(bool show);

    void setShowTotalColumn(bool show);

    QAction *getDeleteAction() const;

    bool hasSelectedProject() const;

    Project getSelectedProject() const;

    void selectFirstRow();

public slots:

    void refresh();

    void selectProject(const Project &project);

    void setShowArchived(bool showArchived);

private slots:

    void onCustomContextMenuRequested(const QPoint &pos);

    void onCurrentChanged(const QModelIndex &index, const QModelIndex &prev);

    void projectUpdated(const Project &project);

    void projectsStatusChanged(const QStringList &projectIDs);

    void deleteSelectedProject();

private:
    void createNewProject(const Project &parentProject);

    void showContextMenu(ProjectTreeItem *item, const QPoint &globalPos);

protected:
    void dragMoveEvent(QDragMoveEvent *event) override;

private:

    TomControl *_control{};
    ProjectStatusManager *_statusManager{};

    ProjectTreeModel *_sourceModel{};
    QSortFilterProxyModel *_proxyModel{};

    QAction *_deleteSelectedAction;
};


#endif //TOM_UI_PROJECTTREEVIEW_H
