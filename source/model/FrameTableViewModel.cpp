#include <QColor>

#include "FrameTableViewModel.h"
#include "UserRoles.h"

const auto redColorValue = QVariant(QColor(Qt::darkRed));
const auto alignedRightVCenter = QVariant(Qt::AlignRight + Qt::AlignVCenter);

FrameTableViewModel::FrameTableViewModel(TomControl *control, QObject *parent) : QAbstractTableModel(parent),
                                                                                 _control(control) {

    connect(_control, &TomControl::framesRemoved, this, &FrameTableViewModel::onFramesRemoved);
    connect(_control, &TomControl::framesMoved, this, &FrameTableViewModel::onFramesMoved);
    connect(_control, &TomControl::framesArchived, this, &FrameTableViewModel::onFramesArchived);
    connect(_control, &TomControl::projectUpdated, this, &FrameTableViewModel::onProjectUpdated);
    connect(_control, &TomControl::projectCreated, this, &FrameTableViewModel::onProjectHierarchyChange);
    connect(_control, &TomControl::projectRemoved, this, &FrameTableViewModel::onProjectHierarchyChange);
    connect(_control, &TomControl::dataResetNeeded, [this] { this->loadFrames(Project()); });

    auto *frameUpdateTimer = new QTimer(this);
    frameUpdateTimer->start(1000);
    connect(frameUpdateTimer, &QTimer::timeout, this, &FrameTableViewModel::onUpdateActiveFrames);
}

FrameTableViewModel::~FrameTableViewModel() {
    qDeleteAll(_frames);
}

void FrameTableViewModel::loadFrames(const Project &project) {
    beginResetModel();

    _currentProject = project;
    qDeleteAll(_frames);

    if (project.isValid()) {
        _frames = _control->loadFrames(project.getID(), true, _showArchived);
    } else {
        _frames.clear();
    }

    endResetModel();

    emit subprojectStatusChange(_control->hasSubprojects(project));
}

void FrameTableViewModel::onProjectHierarchyChange() {
    emit subprojectStatusChange(_control->hasSubprojects(_currentProject));
}

void FrameTableViewModel::onFramesRemoved(const QStringList &frameIDs, const QString &projectID) {
    qDebug() << "frames removed of" << projectID << "if matching" << _currentProject.getID();
    if (!_control->isChildProject(projectID, _currentProject.getID())) {
        return;
    }

    // fixme this loop is possible to optimize to do less view updates
    for (const auto &id : frameIDs) {
        int row = findRow(id);
        if (row >= 0) {
            removeRow(row);
        }
    }
}

void FrameTableViewModel::onFramesMoved(const QStringList &frameIDs, const QString &oldProjectID,
                                        const QString &newProjectID) {
    qDebug() << "frames moved from" << oldProjectID << "to" << newProjectID;

    // don't remove from list if old and new project are in the hierarchy of the currently shown project
    // we have to update the project column, though
    if (_control->isChildProject(oldProjectID, _currentProject.getID()) &&
        _control->isChildProject(newProjectID, _currentProject.getID())) {
        for (const auto &id : frameIDs) {
            int row = findRow(id);
            if (row >= 0) {
                _frames.at(row)->projectID = newProjectID;
                emit dataChanged(createIndex(row, COL_SUBPROJECT), createIndex(row, COL_SUBPROJECT));
            }
        }
    } else {
        // otherwise we'll remove the items from the view
        for (const auto &id : frameIDs) {
            int row = findRow(id);
            if (row >= 0) {
                removeRow(row);
            }
        }
    }
}

bool FrameTableViewModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row >= _frames.size() || row + count - 1 >= _frames.size()) {
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
    if (_control->isChildProject(project.getID(), _currentProject.getID())) {
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
            case COL_SUBPROJECT:
                return "Subproject";
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
            case COL_SUBPROJECT: {
                //remove prefix of current project?
                const QString &parentName = _currentProject.getName();
                QString name = _control->cachedProject(frame->projectID).getName();
                if (parentName == name) {
                    return "";
                }
                return name.remove(parentName + "/");
            }
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
                return redColorValue;
            }
        }
    }

    if (role == Qt::TextAlignmentRole) {
        // right align the end column
        if (index.column() == COL_END) {
            return alignedRightVCenter;
        }
        // right align the duration
        if (index.column() == COL_DURATION) {
            return alignedRightVCenter;
        }
    }

    if (role == SortValueRole) {
        Frame *frame = _frames.at(index.row());
        if (index.column() == COL_START_DATE || index.column() == COL_START) {
            return frame->startTime;
        }
        if (index.column() == COL_END) {
            return frame->stopTime;
        }
        if (index.column() == COL_DURATION) {
            return frame->durationMillis(true);
        }
    }

    if (role == IDRole) {
        Frame *frame = _frames.at(index.row());
        return frame->id;
    }

    return QVariant();
}

Qt::ItemFlags FrameTableViewModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.isValid() &&
        (index.column() != COL_DURATION && index.column() != COL_TAGS && index.column() != COL_SUBPROJECT)) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
    }

    return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;
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
            ok = _control->updateFrame(QList<Frame *>() << frame,
                                       true, startTime,
                                       false, QDateTime(),
                                       false, "",
                                       false, "",
                                       false, false);
            break;
        }
        case COL_END:
            endTime = value.toDateTime();
            ok = _control->updateFrame(QList<Frame *>() << frame,
                                       false, QDateTime(),
                                       true, endTime,
                                       false, "",
                                       false, "",
                                       false, false);
            break;
        case COL_NOTES:
            notes = value.toString();
            ok = _control->updateFrame(QList<Frame *>() << frame,
                                       false, QDateTime(),
                                       false, QDateTime(),
                                       true, notes,
                                       false, "",
                                       false, false);
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

Qt::DropActions FrameTableViewModel::supportedDragActions() const {
    return Qt::MoveAction;
}

QStringList FrameTableViewModel::mimeTypes() const {
    return QStringList() << FRAMES_MIME_TYPE;
}

QMimeData *FrameTableViewModel::mimeData(const QModelIndexList &indexes) const {
    if (!_currentProject.isValid()) {
        return nullptr;
    }

    QStringList ids;
    ids << _currentProject.getID();
    for (auto index : indexes) {
        if (index.isValid()) {
            ids << index.data(IDRole).toString();
        }
    }

    qDebug() << "prepared drag of " << ids;
    auto *mime = new QMimeData();
    mime->setData(FRAMES_MIME_TYPE, ids.join("||").toUtf8());
    return mime;
}

void FrameTableViewModel::onUpdateActiveFrames() {
    const auto &frames = _frames;
    int size = frames.count();

    for (int i = 0; i < size; i++) {
        if (frames.at(i)->isActive()) {
            emit dataChanged(createIndex(i, COL_DURATION), createIndex(i, COL_DURATION));
        }
    }
}

void FrameTableViewModel::setShowArchived(bool showArchived) {
    if (showArchived != _showArchived) {
        _showArchived = showArchived;
        loadFrames(_currentProject);
    }
}

void FrameTableViewModel::onFramesArchived(const QStringList &projectIDs) {
    if (_currentProject.isValid() && projectIDs.contains(_currentProject.getID())) {
        loadFrames(_currentProject);
    }
}
