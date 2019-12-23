#ifndef TOM_UI_TRISTATE_H
#define TOM_UI_TRISTATE_H

#include <QtCore/QString>

typedef enum {
    UNDEFINED = -1,
    FALSE = 0,
    TRUE = 1,
} TriState;

QString tristateString(TriState value);

#endif //TOM_UI_TRISTATE_H
