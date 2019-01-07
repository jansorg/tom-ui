//
// Created by jansorg on 07.01.19.
//

#include "frametableviewmodel.h"

FrameTableViewModel::FrameTableViewModel(QList<Frame *> &frames, QObject *parent) : _frames(frames),
                                                                                    QAbstractTableModel(parent) {

}

FrameTableViewModel::~FrameTableViewModel() {
    qDeleteAll(_frames);
}

int FrameTableViewModel::rowCount(const QModelIndex &parent) const {
    return _frames.size();
}

int FrameTableViewModel::columnCount(const QModelIndex &parent) const {
    return 5;
}

QVariant FrameTableViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return "Start";
            case 1:
                return "End";
            case 2:
                return "Duration";
            case 3:
                return "Tags";
            case 4:
                return "Notes";
            default:
                break;
        }
    }

    return QVariant();
}

QVariant FrameTableViewModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        int row = index.row();
        switch (index.column()) {
            case 0:
                return _frames.at(row)->startTime;
            case 1:
                return _frames.at(row)->stopTime;
            case 2:
                return _frames.at(row)->getDuration().format();
            case 3:
                return _frames.at(row)->tags;
            case 4:
                return _frames.at(row)->notes;
            default:
                break;
        }
    }

    if (role == Qt::TextAlignmentRole) {
        // right align the duration
        if (index.column() == 2) {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
    }

    return QVariant();
}
