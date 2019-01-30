#ifndef GOTIME_UI_GOTIMESTATUS_H
#define GOTIME_UI_GOTIMESTATUS_H

#include <QtCore/QDateTime>
#include "data/Project.h"

class TomStatus {
public:
    explicit TomStatus();

    explicit TomStatus(bool valid, Project &activeProject, QDateTime &startTime);

    const Project &currentProject() const;

    const QDateTime &startTime() const;

    bool isValid;

private:
    Project _project;
    QDateTime _startTime;
};

#endif 
