#include "GotimeStatus.h"

GotimeStatus::GotimeStatus(Project *project, QDateTime &startTime) : _project(project), _startTime(startTime) {
}

const Project *GotimeStatus::currentProject() {
    return _project;
}

const QDateTime &GotimeStatus::startTime() {
    return _startTime;
}
