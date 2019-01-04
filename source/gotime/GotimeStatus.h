//
// Created by jansorg on 04.01.19.
//

#ifndef GOTIME_UI_GOTIMESTATUS_H
#define GOTIME_UI_GOTIMESTATUS_H


#include <data/Project.h>

class GotimeStatus {
public:
    explicit GotimeStatus(Project *activeProject, QDateTime &startTime);

    const Project* currentProject();
    const QDateTime& startTime();

private:
    const Project* _project;
    const QDateTime _startTime;
};


#endif //GOTIME_UI_GOTIMESTATUS_H
