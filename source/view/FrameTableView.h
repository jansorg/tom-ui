#ifndef TOM_UI_FRAMETABLEVIEW_H
#define TOM_UI_FRAMETABLEVIEW_H

#include <QtCore/QArgument>
#include <QtWidgets/QTableView>

#include "gotime/TomControl.h"
#include "data/Project.h"
#include "model/FrameTableViewModel.h"

class FrameTableView : public QTableView {
Q_OBJECT
public:
    explicit FrameTableView(QWidget *parent);

    void setup(TomControl *control);

public slots:

    void onProjectSelected(const Project &project);

    void deleteSelectedEntries();

protected:
    int sizeHintForColumn(int column) const override;

private slots:

    void onCustomContextMenuRequested(const QPoint &pos);

private:
    TomControl *_control;
    QSortFilterProxyModel *_sortedModel;
    FrameTableViewModel *_sourceModel;

    void showContextMenu(Frame *frame, QPoint globalPos);
};

#endif //TOM_UI_FRAMETABLEVIEW_H
