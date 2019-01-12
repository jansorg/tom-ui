#include "gotime_status.h"

gotime_status::gotime_status() : isValid(false), _project(Project()), _startTime(QDateTime()) {

}

gotime_status::gotime_status(bool valid, Project &project, QDateTime &startTime) : isValid(valid),
                                                                                 _project(project),
                                                                                 _startTime(startTime) {
}


const Project &gotime_status::currentProject() const {
    return _project;
}

const QDateTime &gotime_status::startTime() const {
    return _startTime;
}
