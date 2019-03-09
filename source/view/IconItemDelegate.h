#ifndef TOM_UI_ICONITEMDELEGATE_H
#define TOM_UI_ICONITEMDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include "FrameTableView.h"

class IconItemDelegate : public QStyledItemDelegate {
public:
    IconItemDelegate(const QIcon &icon, FrameTableView *parent);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QIcon _icon;
    FrameTableView *_view;
};

#endif //TOM_UI_ICONITEMDELEGATE_H
