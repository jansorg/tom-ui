#ifndef TOM_UI_ICONS_H
#define TOM_UI_ICONS_H

namespace Icons {
    inline QIcon exit() {
        return QIcon::fromTheme("application-exit", QIcon(":/images/start.svg"));
    }

    inline QIcon about() {
        return QIcon::fromTheme("help-about", QIcon(":/images/start.svg"));
    }

    inline QIcon newProject() {
        return QIcon(":/oxygen/tab-new-background.svg");
    }

    inline QIcon start() {
        return QIcon(":/images/start.svg");
    }

    inline QIcon stop() {
        return QIcon(":/images/stop.svg");
    }

    inline QIcon removeFrame() {
        return QIcon(":/images/remove.svg");
    }
}

#endif //TOM_UI_ICONS_H
