#ifndef TOM_UI_TOMSETTINGS_H
#define TOM_UI_TOMSETTINGS_H

#include <QtCore>
#include <QtCore/QSettings>
#include <QtWidgets/QAction>

class TomSettings : public QObject {
Q_OBJECT

public:
    explicit TomSettings(QObject *parent) : QObject(parent) {};

    bool showArchivedEntries();

    bool openMainWindowAtStartup();

    void triggerUpdate();

    void loadAction(QAction *action);

    void saveAction(QAction *action);

signals:

    void onShowArchivedEntriesChanged(bool show);

public slots:

    void setShowArchivedEntries(bool show);

    void setOpenMainWindowAtStartup(bool show);

};


#endif //TOM_UI_TOMSETTINGS_H
