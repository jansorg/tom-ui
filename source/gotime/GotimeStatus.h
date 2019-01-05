#ifndef GOTIME_UI_GOTIMESTATUS_H
#define GOTIME_UI_GOTIMESTATUS_H

#include <data/Project.h>

class GotimeStatus {
public:
    explicit GotimeStatus();
    explicit GotimeStatus(bool valid, Project *activeProject, QDateTime &startTime);

    ~GotimeStatus();

    const Project &currentProject() const;

    const QDateTime &startTime() const;

    const bool isValid;

private:
    const Project* _project;
    const QDateTime _startTime;
};

#endif 
