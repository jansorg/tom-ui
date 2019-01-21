#include <QColor>

#include "frametableviewmodel.h"

FrameTableViewModel::FrameTableViewModel(GotimeControl *control, QObject *parent) : QAbstractTableModel(parent), _control(control) {

    connect(_control, &GotimeControl::frameRemoved, this, &FrameTableViewModel::onFrameRemoved);
    connect(_control, &GotimeControl::projectUpdated, this, &FrameTableViewModel::onProjectUpdated);
}

FrameTableViewModel::~FrameTableViewModel() {
    qDeleteAll(_frames);
}

void FrameTableViewModel::loadFrames(const Project &project) {
    beginResetModel();

    _currentProject = project;
    qDeleteAll(_frames);

    if (project.isValid()) {
        _frames = _control->loadFrames(project.getID(), true);
    } else {
        _frames.clear();
    }

    endResetModel();
}

void FrameTableViewModel::onFrameRemoved(const QString &frameID, const QString &projectID) {
    if (projectID == _currentProject.getID()) {
        int row = findRow(frameID);
        if (row >= 0) {
            removeRow(row);
        }
    }
}

bool FrameTableViewModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row >= _frames.size() || row + count -1 >= _frames.size()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++) {
        _frames.removeAt(row + i);
    }

    endRemoveRows();
    return true;
}

void FrameTableViewModel::onProjectUpdated(const Project &project) {
    if (project == _currentProject) {
        loadFrames(project);
    }
}

int FrameTableViewModel::rowCount(const QModelIndex &) const {
    return _frames.size();
}

int FrameTableViewModel::columnCount(const QModelIndex &) const {
    return COLUMN_COUNT;
}

QVariant FrameTableViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case COL_START_DATE:
                return "Day";
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
        if (section == COL_START_DATE || section == COL_START) {
            return Qt::AlignLeading;
        }
        if (section == COL_DURATION || section == COL_END) {
            return Qt::AlignTrailing;
        }
    }

    return QVariant();
}

Frame *FrameTableViewModel::frameAt(const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    Frame *frame = _frames.at(index.row());
    return frame;
}

QVariant FrameTableViewModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        Frame *frame = _frames.at(index.row());

        switch (index.column()) {
            case COL_START_DATE:
                return frame->startTime.date().toString(Qt::SystemLocaleShortDate);
            case COL_START:
                return frame->startTime.time().toString(Qt::SystemLocaleShortDate);
            case COL_END:
                if (frame->isSpanningMultipleDays()) {
                    return frame->stopTime.toString(Qt::SystemLocaleShortDate);
                }
                return frame->stopTime.time().toString(Qt::SystemLocaleShortDate);
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

    if (role == Qt::EditRole) {
        Frame *frame = _frames.at(index.row());

        switch (index.column()) {
            case COL_START:
                return frame->startTime;
            case COL_END:
                return frame->stopTime;
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
        // right align the end column
        if (index.column() == COL_END) {
            return Qt::AlignRight + Qt::AlignVCenter;
        }
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

int FrameTableViewModel::findRow(const QString &frameID) {
    int index = 0;
    for (const auto *frame : _frames) {
        if (frame->id == frameID) {
            return index;
        }
        index++;
    }
    return -1;
}
