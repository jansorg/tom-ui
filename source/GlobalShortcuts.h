#ifndef TOM_UI_GLOBALSHORTCUTS_H
#define TOM_UI_GLOBALSHORTCUTS_H

#include "gotime/TomControl.h"
#include "GlobalShortcut.h"

class MainWindow;
class XdgPortalGlobalShortcuts;

/**
 * Manages the global shortcuts of the application.
 *
 * On Wayland, the shortcuts are registered with the XDG desktop portal if it's available.
 * Elsewhere, the native APIs of X11, macOS and Windows are used.
 */
class GlobalShortcuts : public QObject {
Q_OBJECT
public:
    explicit GlobalShortcuts(TomControl *control, QObject *parent);

    void setupShortcuts(MainWindow *window);

    QList<GlobalShortcut *> globalShortcuts() const;

    /**
     * @return true if the desktop environment manages the global shortcuts, i.e. the shortcuts
     * defined in the application are just preferences.
     */
    bool isManagedByDesktop() const;

    /**
     * @return the description of the trigger as it was assigned by the desktop environment.
     * Empty if unknown or if the shortcuts are not managed by the desktop.
     */
    QString assignedTrigger(const GlobalShortcut *shortcut) const;

    bool canConfigureInDesktop() const;

    void configureInDesktop();

Q_SIGNALS:
    void assignedTriggersChanged();

private:
    GlobalShortcut *addShortcut(const QString &id, const QString &label, const QKeySequence &defaultShortcut);

    void updatePortalShortcuts();

    TomControl *_control;
    MainWindow *_mainWindow;
    QList<GlobalShortcut *> _shortcuts;
    XdgPortalGlobalShortcuts *_portal;
};


#endif //TOM_UI_GLOBALSHORTCUTS_H
