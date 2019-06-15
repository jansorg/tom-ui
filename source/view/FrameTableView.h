#ifndef TOM_UI_FRAMETABLEVIEW_H
#define TOM_UI_FRAMETABLEVIEW_H

#include <QtCore/QArgument>
#include <QtWidgets/QTableView>

#include "data/Project.h"
#include "gotime/TomControl.h"
#include "gotime/ProjectStatusManager.h"
#include "model/FrameTableViewModel.h"

class FrameTableView : public QTableView {
Q_OBJECT

public:
    explicit FrameTableView(QWidget *parent);

    void setup(TomControl *control, ProjectStatusManager* statusManager);

    QAction *getDeleteAction();

    QList<Frame *> selectedFrames() const;

    void readSettings();

    void writeSettings();

public slots:

    void onProjectSelected(const Project &project);

    void deleteSelectedEntries();

    void archiveSelectedEntries();

    void setShowArchived(bool showArchived);

protected:
    int sizeHintForColumn(int column) const override;

    void startDrag(Qt::DropActions supportedActions) override;

private slots:

    void onCustomContextMenuRequested(const QPoint &pos);

    void onSubprojectStatusChange(bool available);

private:
    TomControl *_control;
    QSortFilterProxyModel *_proxyModel;
    FrameTableViewModel *_sourceModel;
    ProjectStatusManager *_statusManager;

    void showContextMenu(Frame *frame, QPoint globalPos);

    QAction *_deleteSelectedAction;
};

#endif //TOM_UI_FRAMETABLEVIEW_H
