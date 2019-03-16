#ifndef TOM_UI_CHECKBOXCOMBOBOX_H
#define TOM_UI_CHECKBOXCOMBOBOX_H

#include <QtWidgets/QComboBox>
#include <source/view/ProjectTreeView.h>

class ProjectTreeComboBox : public QComboBox {
Q_OBJECT
public:
    explicit ProjectTreeComboBox(QWidget *parent);

    void setup(TomControl *control, ProjectStatusManager *statusManager, const QList<Project> &hiddenProjects = QList<Project>());

    bool eventFilter(QObject *watched, QEvent *event) override;

    void hidePopup() override;

    void setSelectedProject(const Project &project);

    void setSelectedProject(const QString &id);

    Project selectedProject();

    bool _skipNextHide;

signals:

    void indexSelected(const QModelIndex &index);

private:
    TomControl *_control;
    ProjectTreeModel *_sourceModel;
    QSortFilterProxyModel *_sortModel;
    ProjectTreeView *_view;
};

#endif //TOM_UI_CHECKBOXCOMBOBOX_H
