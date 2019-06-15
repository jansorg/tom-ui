#include <QColor>
#include <source/icons.h>

#include "FrameTableViewModel.h"
#include "UserRoles.h"

const auto redColorValue = QVariant(QColor(Qt::darkRed));
const auto alignedRightVCenter = QVariant(Qt::AlignRight + Qt::AlignVCenter);

FrameTableViewModel::FrameTableViewModel(TomControl *control, QObject *parent) : QAbstractTableModel(parent),
                                                                                 _control(control),
                                                                                 _archiveIcon(Icons::timeEntryArchive().pixmap(16, 16, QIcon::Disabled)) {

    connect(_control, &TomControl::framesUpdated, this, &FrameTableViewModel::onFramesUpdates);
    connect(_control, &TomControl::framesRemoved, this, &FrameTableViewModel::onFramesRemoved);
    connect(_control, &TomControl::framesMoved, this, &FrameTableViewModel::onFramesMoved);
    connect(_control, &TomControl::framesArchived, this, &FrameTableViewModel::onFramesArchived);
    connect(_control, &TomControl::projectFramesArchived, this, &FrameTableViewModel::onProjectFramesArchived);

    connect(_control, &TomControl::projectStatusChanged, this, &FrameTableViewModel::onProjectStatusChanged);
    connect(_control, &TomControl::projectUpdated, this, &FrameTableViewModel::onProjectUpdated);
    connect(_control, &TomControl::projectCreated, this, &FrameTableViewModel::onProjectHierarchyChange);
    connect(_control, &TomControl::projectRemoved, this, &FrameTableViewModel::onProjectHierarchyChange);
    connect(_control, &TomControl::dataResetNeeded, [this] { this->loadFrames(Project()); });

    _frameUpdateTimer = new QTimer(this);
    connect(_frameUpdateTimer, &QTimer::timeout, this, &FrameTableViewModel::onUpdateActiveFrames);
}

FrameTableViewModel::~FrameTableViewModel() {
    qDeleteAll(_frames);
}

void FrameTableViewModel::loadFrames(const Project &project) {
    qDebug() << "loading frames" << project.getID();

    stopTimer();

    beginResetModel();

    _currentProject = project;
    qDeleteAll(_frames);
    _frames.clear();

    _frames = _control->loadFrames(project.getID(), true, _showArchived);

    endResetModel();

    emit subprojectStatusChange(_control->hasSubprojects(project));

    if (project.isValidOrRootProject()) {
        // start the timer only if we're showing active frames
        for (auto *f : _frames) {
            if (f->isActive()) {
                startTimer();
                break;
            }
        }
    }
}

void FrameTableViewModel::onProjectHierarchyChange() {
    emit subprojectStatusChange(_control->hasSubprojects(_currentProject));
}


void FrameTableViewModel::onFramesUpdates(const QStringList &frameIDs, const QStringList &projectIDs) {
    qDebug() << "frames updated of projects" << projectIDs;

    if (!_currentProject.isValidOrRootProject()) {
        return;
    }

    const QString &projectID = _currentProject.getID();
    if (!_currentProject.isRootProject() && !projectIDs.contains(projectID) && !_control->isAnyChildProject(projectIDs, projectID)) {
        return;
    }

    // fixme this can be optimize, we could track the source of the event (it could be us)
    updateFrames(frameIDs);
}

void FrameTableViewModel::onFramesRemoved(const QStringList &frameIDs, const QStringList &projectIDs) {
    qDebug() << "frames removed of" << projectIDs << "if matching" << _currentProject.getID();

    // don't continue if none of the source proejct IDs is matching the current view
    if (!_control->isAnyChildProject(projectIDs, _currentProject.getID())) {
        return;
    }

    removeFrameRows(frameIDs);
}

void FrameTableViewModel::onFramesArchived(const QStringList &frameIDs, const QStringList &projectIDs, bool nowArchived) {
    if (!_currentProject.isValidOrRootProject()) {
        return;
    }

    // return if we're not displaying any of the modified projects
    const QString &currentID = _currentProject.getID();
    if (!_currentProject.isRootProject() && !projectIDs.contains(currentID) && !_control->isAnyChildProject(projectIDs, currentID)) {
        return;
    }

    if (_showArchived) {
        // update the archived status column
        for (const auto &frameID: frameIDs) {
            int row = findRow(frameID);
            if (row >= 0) {
                _frames.at(row)->archived = nowArchived;
                emit dataChanged(createIndex(row, COL_ARCHIVED), createIndex(row, COL_ARCHIVED));
            }
        }
    } else if (nowArchived) {
        // remove all archived frames from the list
        removeFrameRows(frameIDs);
    } else {
        // show all affected frames (which are not archived anymore)
        loadFrames(_currentProject);
    }
}

void FrameTableViewModel::onProjectFramesArchived(const QStringList &projectIDs) {
    if (!_currentProject.isValidOrRootProject()) {
        return;
    }

    // return if we're not displaying any of the modified projects
    const QString &currentID = _currentProject.getID();
    if (!_currentProject.isRootProject() && !projectIDs.contains(currentID) && !_control->isAnyChildProject(projectIDs, currentID)) {
        return;
    }

    if (_showArchived) {
        loadFrames(_currentProject);
    } else {
        // remove all frames which belong to any of the archived projects
        // we make a copy of all affected frames because removeRow will change the list of frames
        QStringList frameIDs;
        for (auto frame : _frames) {
            if (projectIDs.contains(frame->projectID)) {
                frameIDs << frame->id;
            }
        }

        // remove all affected frames
        removeFrameRows(frameIDs);
    }
}

void FrameTableViewModel::onFramesMoved(const QStringList &frameIDs, const QStringList &oldProjectIDs, const QString &newProjectID) {
    qDebug() << "frames moved from" << oldProjectIDs << "to" << newProjectID;

    // don't remove from list if old and new project are in the hierarchy of the currently shown project
    // we have to update the project column, though
    if (_currentProject.isRootProject() || (_control->isAnyChildProject(oldProjectIDs, _currentProject.getID()) && _control->isChildProject(newProjectID, _currentProject.getID()))) {
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

void FrameTableViewModel::onProjectStatusChanged(const Project &started, const Project &stopped) {
    if (started.isValid() && (_currentProject.isRootProject() || _control->isChildProject(started.getID(), _currentProject.getID()))) {
        loadFrames(_currentProject);
    } else if (stopped.isValid() && (_currentProject.isRootProject() || _control->isChildProject(stopped.getID(), _currentProject.getID()))) {
        loadFrames(_currentProject);
    }
}

void FrameTableViewModel::onProjectUpdated(const Project &project) {
    if (_currentProject.isRootProject() || _control->isChildProject(project.getID(), _currentProject.getID())) {
        loadFrames(_currentProject);
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
                return tr("Day");
            case COL_START:
                return tr("Start");
            case COL_END:
                return tr("End");
            case COL_DURATION:
                return tr("Duration");
            case COL_TAGS:
                return tr("Tags");
            case COL_SUBPROJECT:
                return tr("Subproject");
            case COL_ARCHIVED:
                return "";
            case COL_NOTES:
                return tr("Notes");
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

QModelIndex FrameTableViewModel::buddy(const QModelIndex &index) const {
    if (index.isValid() && index.column() == COL_START_DATE) {
        return index.siblingAtColumn(COL_START);
    }
    return QAbstractItemModel::buddy(index);
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
            case COL_ARCHIVED:
                return frame->archived ? _archiveIcon : QVariant();
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

                if (parentName == "") {
                    return name;
                }

                if (parentName == name) {
                    return "";
                }

                return name.remove(0, 1 + parentName.length());
            }
            case COL_NOTES:
                return QString(frame->notes).replace(QRegularExpression("\r\n|\r|\n"), " ");
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
            case COL_ARCHIVED:
                return frame->archived;
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
        if (index.column() == COL_ARCHIVED) {
            return frame->archived;
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

    if (index.column() == COL_ARCHIVED) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled;
    }

    if (index.column() != COL_DURATION && index.column() != COL_TAGS && index.column() != COL_SUBPROJECT) {
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

    bool ok = false;
    switch (col) {
        case COL_ARCHIVED: {
            if (value.canConvert(QVariant::Bool)) {
                bool isArchived = value.toBool();
                if (frame->archived != isArchived) {
                    ok = _control->updateFrame(QList<Frame *>() << frame,
                                               false, QDateTime(),
                                               false, QDateTime(),
                                               false, "",
                                               false, "",
                                               true, isArchived);
                }
            }
            break;
        }
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
    // indexes contains every column of each dragged row, we must send a row only once
    QSet<QString> frameIDs;
    QSet<QString> projectIDs;

    for (auto index : indexes) {
        if (index.isValid()) {
            if (Frame *frame = frameAt(index)){
                projectIDs << frame->projectID;
                frameIDs << frame->id;
            }
        }
    }

    // the format is:
    // list of strings
    // 1st item: comma-seperated list of project IDs
    // all other items are ids of the dragged frames

    auto *mime = new QMimeData();
    QStringList items = QStringList() << projectIDs.toList().join(',') << frameIDs.toList();
    mime->setData(FRAMES_MIME_TYPE, items.join("||").toUtf8());
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

void FrameTableViewModel::startTimer() {
    if (!_frameUpdateTimer->isActive()) {
        _frameUpdateTimer->start(1000);
    }
}

void FrameTableViewModel::stopTimer() {
    _frameUpdateTimer->stop();
}

void FrameTableViewModel::removeFrameRows(const QStringList &ids) {
// fixme this loop is possible to optimize to do less view updates
    for (const auto &id : ids) {
        int row = findRow(id);
        if (row >= 0) {
            removeRow(row);
        }
    }
}

void FrameTableViewModel::updateFrames(const QStringList &ids) {
    //fixme optimize by only loading necessary frames
    const QList<Frame *> &allFrames = _control->loadFrames(_currentProject.getID(), true, _showArchived);

    for (const auto &id: ids) {
        int row = findRow(id);
        if (row >= 0) {
            // locate frame data and update the underlying data
            // fixme this is slow, esp. for a larger list of ids
            Frame* current = _frames[row];
            for (auto frame : allFrames) {
                if (id == frame->id) {
                    *current = *frame;
                    emit dataChanged(createIndex(row, FIRST_COL), createIndex(row, LAST_COL));
                }
            }
        }
    }

    //delete the frames created by loadFrames()!
    qDeleteAll(allFrames);
}
