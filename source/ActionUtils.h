#ifndef TOM_UI_ACTIONUTILS_H
#define TOM_UI_ACTIONUTILS_H


#include "gotime/TomControl.h"

class ActionUtils {
public:
    static bool removeProject(TomControl *control, const Project &project, QWidget* parent);
};


#endif //TOM_UI_ACTIONUTILS_H
