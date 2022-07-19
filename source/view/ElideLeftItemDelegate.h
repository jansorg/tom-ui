#ifndef TOM_UI_ELIDELEFTELEGATE_H
#define TOM_UI_ELIDELEFTELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

class ElideLeftItemDelegate : public QStyledItemDelegate {

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif //TOM_UI_ELIDELEFTELEGATE_H
