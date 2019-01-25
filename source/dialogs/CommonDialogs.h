//
// Created by jansorg on 19.01.19.
//

#ifndef TOM_UI_COMMONDIALOGS_H
#define TOM_UI_COMMONDIALOGS_H

#include <gotime/TomControl.h>
#include "data/Project.h"

class CommonDialogs {
public:
    static Project createProject(const Project& parentProject, TomControl* control, QWidget* parent);
};


#endif //TOM_UI_COMMONDIALOGS_H
