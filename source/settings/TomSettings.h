#ifndef TOM_UI_TOMSETTINGS_H
#define TOM_UI_TOMSETTINGS_H

#include <QtCore>
#include <QtCore/QSettings>
#include <QtWidgets/QAction>

class TomSettings : public QObject {
Q_OBJECT

public:
    explicit TomSettings(QObject *parent) : QObject(parent) {};

    bool openMainWindowAtStartup();

    void loadAction(QAction *action);

    void saveAction(QAction *action);

public slots:

    void setOpenMainWindowAtStartup(bool show);
};


#endif //TOM_UI_TOMSETTINGS_H
