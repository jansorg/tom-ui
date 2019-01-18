#ifndef TOM_UI_ICONS_H
#define TOM_UI_ICONS_H

namespace Icons {
    inline QIcon exit() {
        return QIcon::fromTheme("application-exit", QIcon(":/images/start.svg"));
    }

    inline QIcon about() {
        return QIcon::fromTheme("help-about", QIcon(":/images/start.svg"));
    }
}

#endif //TOM_UI_ICONS_H
