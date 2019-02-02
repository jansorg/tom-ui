//
// Created by jansorg on 28.01.19.
//

#ifndef TOM_UI_REPORTSPLITMODEL_H
#define TOM_UI_REPORTSPLITMODEL_H

#include <QStringListModel>
#include <QtCore/QSet>

class ReportSplitModel : public QStringListModel {
Q_OBJECT
public:
    explicit ReportSplitModel(QObject* parent);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void setCheckedItems(const QStringList &items);

    QStringList checkedItems();

    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationRow) override;

signals:

    void itemStateChanged();

private:
    QSet<QString> _checked;
};


#endif //TOM_UI_REPORTSPLITMODEL_H
