//
// Created by jansorg on 16.01.19.
//

#ifndef TOM_UI_PROJECTTREEVIEW_H
#define TOM_UI_PROJECTTREEVIEW_H

#include <QtWidgets/QAction>
#include <QtWidgets/QTreeView>

#include "model/project_tree_item.h"
#include "model/ProjectTreeModel.h"
#include "gotime/GotimeControl.h"

class ProjectTreeView : public QTreeView {
    Q_OBJECT

public:
    explicit ProjectTreeView(QWidget *parent);

    void setControl(GotimeControl *control);

    void refresh();

signals:

    void projectSelected(const Project &project);

protected:
    ProjectTreeModel *getProjectModel();

private slots:

    void onCustomContextMenuRequested(const QPoint &pos);

    void onProjectSelected(const QModelIndex &index);

    void projectUpdated(const Project &project);

private:
    void showContextMenu(ProjectTreeItem *item, const QPoint &globalPos);

    GotimeControl *_control;
};


#endif //TOM_UI_PROJECTTREEVIEW_H
