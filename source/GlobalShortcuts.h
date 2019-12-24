#ifndef TOM_UI_GLOBALSHORTCUTS_H
#define TOM_UI_GLOBALSHORTCUTS_H

#include <qxt/qxtglobalshortcut.h>

#include "gotime/TomControl.h"
#include "GlobalShortcut.h"

class MainWindow;

class GlobalShortcuts : public QObject {
Q_OBJECT
public:
    explicit GlobalShortcuts(TomControl *control, QObject *parent);

    void setupShortcuts(MainWindow *window);

    QList<GlobalShortcut *> globalShortcuts() const;

private:
    TomControl *_control;
    MainWindow *_mainWindow;
    QList<GlobalShortcut *> _shortcuts;
};


#endif //TOM_UI_GLOBALSHORTCUTS_H
