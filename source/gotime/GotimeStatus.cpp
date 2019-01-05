#include "GotimeStatus.h"

GotimeStatus::GotimeStatus() : isValid(false), _project(Project()), _startTime(QDateTime()) {

}

GotimeStatus::GotimeStatus(bool valid, Project &project, QDateTime &startTime) : isValid(valid),
                                                                                 _project(project),
                                                                                 _startTime(startTime) {
}


const Project &GotimeStatus::currentProject() const {
    return _project;
}

const QDateTime &GotimeStatus::startTime() const {
    return _startTime;
}
