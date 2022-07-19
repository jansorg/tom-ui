#include "ElideLeftItemDelegate.h"

#include <QPainter>
#include "FrameTableView.h"

void ElideLeftItemDelegate::paint(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    opt.textElideMode = Qt::ElideLeft;
    QStyledItemDelegate::paint(painter, opt, index);
}
