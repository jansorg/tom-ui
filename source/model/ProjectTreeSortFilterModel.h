//
// Created by jansorg on 21.01.19.
//

#ifndef TOM_UI_PROJECTTREESORTFILTERMODEL_H
#define TOM_UI_PROJECTTREESORTFILTERMODEL_H


#include <QtCore/QSortFilterProxyModel>

class ProjectTreeSortFilterModel : public QSortFilterProxyModel {
public:
    explicit ProjectTreeSortFilterModel(QObject *parent);

    void sort(int column, Qt::SortOrder order) override;

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};


#endif //TOM_UI_PROJECTTREESORTFILTERMODEL_H
