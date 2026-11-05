#ifndef TOM_UI_XDGPORTALGLOBALSHORTCUTS_H
#define TOM_UI_XDGPORTALGLOBALSHORTCUTS_H

#include <functional>

#include <QObject>
#include <QHash>
#include <QKeySequence>
#include <QTimer>
#include <QVariantMap>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

/**
 * Global shortcuts via the XDG desktop portal (org.freedesktop.portal.GlobalShortcuts).
 *
 * This is the only way to register global shortcuts on Wayland. The desktop environment owns the
 * actual key bindings: the key sequences passed here are only sent as preferred triggers and
 * the user may change them in the system settings at any time.
 *
 * See https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.GlobalShortcuts.html
 */
class XdgPortalGlobalShortcuts : public QObject {
Q_OBJECT

public:
    struct Shortcut {
        QString id;
        QString description;
        QKeySequence preferredTrigger;
    };

    /**
     * @return a new instance if the session is a Wayland session and the portal supports global shortcuts,
     * nullptr otherwise.
     */
    static XdgPortalGlobalShortcuts *create(const QString &appId, QObject *parent);

    ~XdgPortalGlobalShortcuts() override;

    /**
     * Replaces the set of bound shortcuts. Rebinding happens asynchronously and multiple calls
     * in the same event loop iteration are merged.
     */
    void setShortcuts(const QList<Shortcut> &shortcuts);

    /**
     * @return the human-readable trigger reported by the desktop environment for the given shortcut,
     * or an empty string if it's unknown.
     */
    QString triggerDescription(const QString &id) const;

    /**
     * @return true if the portal supports opening the system configuration of the shortcuts.
     */
    bool canConfigure() const;

    /**
     * Opens the desktop environment's configuration of our global shortcuts.
     */
    void configure();

    /**
     * Converts the key sequence into the trigger format defined by the XDG shortcuts specification,
     * e.g. "CTRL+SHIFT+p".
     */
    static QString toPortalTrigger(const QKeySequence &sequence);

Q_SIGNALS:
    void activated(const QString &id);

    void triggersChanged();

private Q_SLOTS:
    void onActivated(const QDBusMessage &message);

    void onShortcutsChanged(const QDBusMessage &message);

private:
    XdgPortalGlobalShortcuts(const QDBusConnection &connection, uint version, QObject *parent);

    using ResponseHandler = std::function<void(uint response, const QVariantMap &results)>;

    void rebind();

    void createSession();

    void bindShortcuts();

    void closeSession();

    void callWithRequest(const QString &method, QList<QVariant> args, QVariantMap options,
                         const ResponseHandler &handler);

    void updateTriggers(const QVariant &shortcuts);

    QDBusConnection _connection;
    uint _version;
    QString _sessionHandle;
    bool _requestPending = false;
    bool _rebindPending = false;
    QTimer _rebindTimer;
    QList<Shortcut> _shortcuts;
    QHash<QString, QString> _triggerDescriptions;
};

#endif //TOM_UI_XDGPORTALGLOBALSHORTCUTS_H
