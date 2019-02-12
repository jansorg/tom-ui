#ifndef TOM_UI_TOMSETTINGS_H
#define TOM_UI_TOMSETTINGS_H

#include <QtCore>
#include <QtCore/QSettings>

class TomSettings : public QObject {
Q_OBJECT

public:
    explicit TomSettings(QObject *parent) : QObject(parent) {};

    bool showArchivedEntries();

    void triggerUpdate();

signals:

    void onShowArchivedEntriesChanged(bool show);

public slots:

    void setShowArchivedEntries(bool show);

};


#endif //TOM_UI_TOMSETTINGS_H
