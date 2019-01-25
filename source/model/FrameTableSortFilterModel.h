//
// Created by jansorg on 21.01.19.
//

#ifndef TOM_UI_FRAMETABLESORTFILTERMODEL_H
#define TOM_UI_FRAMETABLESORTFILTERMODEL_H


#include <QtCore/QObject>
#include <QtCore/QSortFilterProxyModel>

class FrameTableSortFilterModel : public QSortFilterProxyModel {
public:
    explicit FrameTableSortFilterModel(QObject *parent);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};


#endif //TOM_UI_FRAMETABLESORTFILTERMODEL_H
