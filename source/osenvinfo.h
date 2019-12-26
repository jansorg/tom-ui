#ifndef TOM_UI_OSENVINFO_H
#define TOM_UI_OSENVINFO_H


class OSEnvInfo {
public:
    inline static bool supportsHTMLTooltips() {
#ifndef Q_OS_MAC
        return true;
#else
        return false;
#endif
    }
};


#endif //TOM_UI_OSENVINFO_H
