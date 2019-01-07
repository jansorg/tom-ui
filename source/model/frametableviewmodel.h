//
// Created by jansorg on 07.01.19.
//

#ifndef GOTIME_UI_FRAMETABLEVIEWMODEL_H
#define GOTIME_UI_FRAMETABLEVIEWMODEL_H


#include <QtCore/QAbstractTableModel>
#include <data/frame.h>

class FrameTableViewModel : public QAbstractTableModel {
public:
    explicit FrameTableViewModel(QList<Frame*>& frames, QObject* parent);
    ~FrameTableViewModel() override;

private:
    QList<Frame*> _frames;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};


#endif //GOTIME_UI_FRAMETABLEVIEWMODEL_H
