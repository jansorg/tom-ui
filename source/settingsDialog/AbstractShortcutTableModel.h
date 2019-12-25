#ifndef TOM_UI_ABSTRACTSHORTCUTTABLEMODEL_H
#define TOM_UI_ABSTRACTSHORTCUTTABLEMODEL_H

#include <QtCore/QAbstractTableModel>

class AbstractShortcutTableModel : public QAbstractTableModel {
Q_OBJECT
public:
    explicit AbstractShortcutTableModel(QObject *parent = nullptr);

    virtual QKeySequence getActionKeySequence(const QModelIndex &index) = 0;

public slots:
    virtual void updateKeySequence(const QModelIndex &index, const QKeySequence &keySequence) = 0;

    QModelIndex parent(const QModelIndex &child) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex &parent) const override;
};


#endif //TOM_UI_ABSTRACTSHORTCUTTABLEMODEL_H
