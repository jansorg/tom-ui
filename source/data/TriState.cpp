#include "TriState.h"

QString tristateString(TriState value) {
    if (value == TRUE) {
        return "true";
    }
    if (value == FALSE) {
        return "false";
    }
    return "";
}
