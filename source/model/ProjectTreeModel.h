#ifndef GOTIME_UI_PROJECTTREEMODEL_H
#define GOTIME_UI_PROJECTTREEMODEL_H


static const char *const PROJECT_MIME_TYPE = "application/x-tom-projects";

#include <QtCore/QAbstractItemModel>

#include "data/Project.h"
#include "gotime/GotimeControl.h"
#include "project_tree_item.h"
#include "projecttreerootitem.h"

class ProjectTreeModel : public QAbstractItemModel {
Q_OBJECT

public:
    ProjectTreeModel(GotimeControl *control, ProjectStatusManager *statusManager, QObject *parent);

    ~ProjectTreeModel() override;

    Project projectAtIndex(const QModelIndex &index);

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent) const override;

    inline int columnCount(const QModelIndex &) const override {
        return ProjectTreeItem::COL_COUNT;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    ProjectTreeItem *getItem(const QModelIndex &index) const;

    void updateProject(const Project &project);

    void updateProjectStatus(const QString &projectID);

    QModelIndex getProjectRow(const QString &projectID) const;

public slots:

    void loadProjects();

    Qt::DropActions supportedDropActions() const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    QStringList mimeTypes() const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

private slots:

    void addProject(const Project &project);

private:
    ProjectTreeRootItem *_rootItem;
    ProjectTreeRootItem *_visibleRootItem;
    GotimeControl *_control;
    QList<Project> _projects;
    ProjectStatusManager *_statusManager;

    QStringList _headers;

    ProjectTreeItem *projectItemAtIndex(const QModelIndex &index);

    bool _blockUpdates;

    void addProjectItems(const QList<Project> &allProjects, ProjectTreeItem *parent);

    void printProjects(int level, ProjectTreeItem *root);
};

#endif //GOTIME_UI_PROJECTTREEMODEL_H
