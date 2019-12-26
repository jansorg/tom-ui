#include "TomStatus.h"

#include <utility>

TomStatus::TomStatus() : isValid(false),
                         _timeEntryId(""),
                         _project(Project()),
                         _startTime(QDateTime()) {
}

TomStatus::TomStatus(bool valid, QString timeEntryId, Project &project, QDateTime &startTime) : isValid(valid),
                                                                                                _timeEntryId(std::move(timeEntryId)),
                                                                                                _project(project),
                                                                                                _startTime(startTime) {
}


const Project &TomStatus::currentProject() const {
    return _project;
}

const QDateTime &TomStatus::startTime() const {
    return _startTime;
}

const QString &TomStatus::timeEntryId() const {
    return _timeEntryId;
}
