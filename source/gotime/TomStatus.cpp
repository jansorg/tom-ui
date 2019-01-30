#include "TomStatus.h"

TomStatus::TomStatus() : isValid(false), _project(Project()), _startTime(QDateTime()) {

}

TomStatus::TomStatus(bool valid, Project &project, QDateTime &startTime) : isValid(valid),
                                                                                 _project(project),
                                                                                 _startTime(startTime) {
}


const Project &TomStatus::currentProject() const {
    return _project;
}

const QDateTime &TomStatus::startTime() const {
    return _startTime;
}
