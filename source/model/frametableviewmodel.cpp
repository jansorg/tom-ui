#include <QColor>

#include "frametableviewmodel.h"

FrameTableViewModel::FrameTableViewModel(QList<Frame *> &frames, GotimeControl *control, QObject *parent) : QAbstractTableModel(parent),
                                                                                                            _frames(frames),
                                                                                                            _control(control) {}

FrameTableViewModel::~FrameTableViewModel() {
    qDeleteAll(_frames);
}

int FrameTableViewModel::rowCount(const QModelIndex &) const {
    return _frames.size();
}

int FrameTableViewModel::columnCount(const QModelIndex &) const {
    return COL_NOTES + 1;
}

QVariant FrameTableViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COL_START:
                return "Start";
            case COL_END:
                return "End";
            case COL_DURATION:
                return "Duration";
            case COL_TAGS:
                return "Tags";
            case COL_NOTES:
                return "Notes";
            default:
                break;
        }
    }

    if (role == Qt::TextAlignmentRole && orientation == Qt::Horizontal) {
        if (section == COL_DURATION) {
            return Qt::AlignRight;
        }
    }

    return QVariant();
}

QVariant FrameTableViewModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        Frame *frame = _frames.at(index.row());

        switch (index.column()) {
            case COL_START:
                return frame->startTime;
            case COL_END:
                return frame->stopTime;
            case COL_DURATION:
                if (!frame->stopTime.isValid()) {
                    return Timespan::of(frame->startTime, QDateTime::currentDateTime()).format();
                }
                return frame->getDuration().format();
            case COL_TAGS:
                return frame->tags;
            case COL_NOTES:
                return frame->notes;
            default:
                break;
        }
    }

    if (role == Qt::TextColorRole) {
        if (index.column() == COL_DURATION) {
            Frame *frame = _frames.at(index.row());
            if (!frame->stopTime.isValid()) {
                return QVariant(QColor(Qt::gray));
            }
        }
    }

    if (role == Qt::TextAlignmentRole) {
        // right align the duration
        if (index.column() == COL_DURATION) {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
    }

    return QVariant();
}

Qt::ItemFlags FrameTableViewModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == COL_DURATION || index.column() == COL_TAGS) {
        return QAbstractTableModel::flags(index);
    }
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool FrameTableViewModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole || !index.isValid()) {
        return false;
    }

    int col = index.column();
    if (col == COL_TAGS || col == COL_DURATION) {
        return false;
    }

    Frame *frame = _frames.at(index.row());
    QDateTime startTime = frame->startTime;
    QDateTime endTime = frame->stopTime;
    QString notes = frame->notes;

    bool ok;
    switch (col) {
        case COL_START: {
            startTime = value.toDateTime();
            ok = _control->updateFrame(frame, true, startTime, false, QDateTime(), false, "");
            break;
        }
        case COL_END:
            endTime = value.toDateTime();
            ok = _control->updateFrame(frame, false, QDateTime(), true, endTime, false, "");
            break;
        case COL_NOTES:
            notes = value.toString();
            ok = _control->updateFrame(frame, false, QDateTime(), false, QDateTime(), true, notes);
            break;
        default:
            ok = false;
    }

    if (ok) {
        frame->startTime = startTime;
        frame->stopTime = endTime;
        frame->notes = notes;
        emit dataChanged(index, index);
    }
    return ok;
}
