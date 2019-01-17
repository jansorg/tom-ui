//
// Created by jansorg on 07.01.19.
//

#ifndef GOTIME_UI_FRAMETABLEVIEWMODEL_H
#define GOTIME_UI_FRAMETABLEVIEWMODEL_H

#include <QtCore/QAbstractTableModel>
#include <gotime/GotimeControl.h>

#include "data/frame.h"

class FrameTableViewModel : public QAbstractTableModel {
public:
    explicit FrameTableViewModel(QList<Frame*>& frames, GotimeControl* control, QObject* parent);
    ~FrameTableViewModel() override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<Frame*> _frames;
    GotimeControl* _control;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    static const int COL_START = 0;
    static const int COL_END = 1;
    static const int COL_DURATION = 2;
    static const int COL_TAGS = 3;
    static const int COL_NOTES = 4;
};


#endif //GOTIME_UI_FRAMETABLEVIEWMODEL_H
