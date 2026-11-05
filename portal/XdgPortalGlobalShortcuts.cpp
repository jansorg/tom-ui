#include "XdgPortalGlobalShortcuts.h"

#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QRandomGenerator>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>
#include <QtDBus/QDBusVariant>

namespace {
    const QString portalService = QStringLiteral("org.freedesktop.portal.Desktop");
    const QString portalPath = QStringLiteral("/org/freedesktop/portal/desktop");
    const QString shortcutsInterface = QStringLiteral("org.freedesktop.portal.GlobalShortcuts");
    const QString requestInterface = QStringLiteral("org.freedesktop.portal.Request");
    const QString sessionInterface = QStringLiteral("org.freedesktop.portal.Session");

    // D-Bus signature (sa{sv})
    struct PortalShortcut {
        QString id;
        QVariantMap properties;
    };
    using PortalShortcutList = QList<PortalShortcut>;

    QDBusArgument &operator<<(QDBusArgument &arg, const PortalShortcut &shortcut) {
        arg.beginStructure();
        arg << shortcut.id << shortcut.properties;
        arg.endStructure();
        return arg;
    }

    const QDBusArgument &operator>>(const QDBusArgument &arg, PortalShortcut &shortcut) {
        arg.beginStructure();
        arg >> shortcut.id >> shortcut.properties;
        arg.endStructure();
        return arg;
    }

    QString newToken() {
        return QStringLiteral("tomui_%1").arg(QRandomGenerator::global()->generate());
    }

    bool isWaylandSession() {
        return QGuiApplication::platformName().startsWith(QLatin1String("wayland"))
               || qEnvironmentVariable("XDG_SESSION_TYPE") == QLatin1String("wayland")
               || qEnvironmentVariableIsSet("WAYLAND_DISPLAY");
    }

    QString keyName(int key) {
        if ((key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9)) {
            return QString(QChar(key).toLower());
        }
        if (key >= Qt::Key_F1 && key <= Qt::Key_F35) {
            return QStringLiteral("F%1").arg(key - Qt::Key_F1 + 1);
        }

        // xkb keysym names, see xkbcommon-keysyms.h
        static const QHash<int, QString> names = {
                {Qt::Key_Escape,       QStringLiteral("Escape")},
                {Qt::Key_Tab,          QStringLiteral("Tab")},
                {Qt::Key_Backspace,    QStringLiteral("BackSpace")},
                {Qt::Key_Return,       QStringLiteral("Return")},
                {Qt::Key_Enter,        QStringLiteral("KP_Enter")},
                {Qt::Key_Insert,       QStringLiteral("Insert")},
                {Qt::Key_Delete,       QStringLiteral("Delete")},
                {Qt::Key_Pause,        QStringLiteral("Pause")},
                {Qt::Key_Print,        QStringLiteral("Print")},
                {Qt::Key_Home,         QStringLiteral("Home")},
                {Qt::Key_End,          QStringLiteral("End")},
                {Qt::Key_Left,         QStringLiteral("Left")},
                {Qt::Key_Up,           QStringLiteral("Up")},
                {Qt::Key_Right,        QStringLiteral("Right")},
                {Qt::Key_Down,         QStringLiteral("Down")},
                {Qt::Key_PageUp,       QStringLiteral("Prior")},
                {Qt::Key_PageDown,     QStringLiteral("Next")},
                {Qt::Key_Space,        QStringLiteral("space")},
                {Qt::Key_Comma,        QStringLiteral("comma")},
                {Qt::Key_Period,       QStringLiteral("period")},
                {Qt::Key_Minus,        QStringLiteral("minus")},
                {Qt::Key_Plus,         QStringLiteral("plus")},
                {Qt::Key_Equal,        QStringLiteral("equal")},
                {Qt::Key_Slash,        QStringLiteral("slash")},
                {Qt::Key_Backslash,    QStringLiteral("backslash")},
                {Qt::Key_Semicolon,    QStringLiteral("semicolon")},
                {Qt::Key_Apostrophe,   QStringLiteral("apostrophe")},
                {Qt::Key_BracketLeft,  QStringLiteral("bracketleft")},
                {Qt::Key_BracketRight, QStringLiteral("bracketright")},
                {Qt::Key_QuoteLeft,    QStringLiteral("grave")},
                {Qt::Key_NumberSign,   QStringLiteral("numbersign")},
                {Qt::Key_Asterisk,     QStringLiteral("asterisk")},
        };
        if (names.contains(key)) {
            return names.value(key);
        }
        return QKeySequence(key).toString(QKeySequence::PortableText);
    }
}

/**
 * Helper to receive the Response signal of a single portal request.
 */
class PortalRequest : public QObject {
    Q_OBJECT
    public:
        using Handler = std::function<void(uint, const QVariantMap &)>;

        PortalRequest(QDBusConnection connection, Handler handler, QObject *parent)
                : QObject(parent), _connection(std::move(connection)), _handler(std::move(handler)) {
        }

        bool listen(const QString &path) {
            if (!_path.isEmpty()) {
                _connection.disconnect(portalService, _path, requestInterface, QStringLiteral("Response"),
                                       this, SLOT(onResponse(uint, QVariantMap)));
            }
            _path = path;
            return _connection.connect(portalService, _path, requestInterface, QStringLiteral("Response"),
                                       this, SLOT(onResponse(uint, QVariantMap)));
        }

        const QString &path() const {
            return _path;
        }

    public Q_SLOTS:

        void onResponse(uint response, const QVariantMap &results) {
            finish(response, results);
        }

        void finish(uint response, const QVariantMap &results) {
            if (_handler) {
                auto handler = std::move(_handler);
                _handler = nullptr;
                handler(response, results);
            }
            deleteLater();
        }

    private:
        QDBusConnection _connection;
        Handler _handler;
        QString _path;
};

Q_DECLARE_METATYPE(PortalShortcut)

XdgPortalGlobalShortcuts *XdgPortalGlobalShortcuts::create(const QString &appId, QObject *parent) {
    if (!isWaylandSession()) {
        return nullptr;
    }

    // A private connection, because the host app registration must happen before any other portal call
    // on the connection and Qt itself may already have used the portal on the shared session bus connection.
    QDBusConnection connection = QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                                               QStringLiteral("tom-ui-global-shortcuts"));
    if (!connection.isConnected()) {
        qWarning() << "Unable to connect to the D-Bus session bus:" << connection.lastError().message();
        return nullptr;
    }

    // Host (i.e. non-sandboxed) apps must tell the portal their app id, it's used to store the shortcuts.
    // This is unsupported by older portals and fails in a sandbox, which is fine.
    if (!appId.isEmpty() && !QFile::exists(QStringLiteral("/.flatpak-info"))) {
        auto registerCall = QDBusMessage::createMethodCall(portalService, portalPath,
                                                           QStringLiteral("org.freedesktop.host.portal.Registry"),
                                                           QStringLiteral("Register"));
        registerCall << appId << QVariantMap();
        connection.call(registerCall, QDBus::Block, 2000);
    }

    auto versionCall = QDBusMessage::createMethodCall(portalService, portalPath,
                                                      QStringLiteral("org.freedesktop.DBus.Properties"),
                                                      QStringLiteral("Get"));
    versionCall << shortcutsInterface << QStringLiteral("version");
    const QDBusMessage reply = connection.call(versionCall, QDBus::Block, 5000);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        qInfo() << "The XDG desktop portal does not support global shortcuts:" << reply.errorMessage();
        QDBusConnection::disconnectFromBus(connection.name());
        return nullptr;
    }

    const uint version = reply.arguments().first().value<QDBusVariant>().variant().toUInt();
    if (version == 0) {
        QDBusConnection::disconnectFromBus(connection.name());
        return nullptr;
    }

    return new XdgPortalGlobalShortcuts(connection, version, parent);
}

XdgPortalGlobalShortcuts::XdgPortalGlobalShortcuts(const QDBusConnection &connection, uint version, QObject *parent)
        : QObject(parent),
          _connection(connection),
          _version(version) {
    qDBusRegisterMetaType<PortalShortcut>();
    qDBusRegisterMetaType<PortalShortcutList>();

    _rebindTimer.setSingleShot(true);
    _rebindTimer.setInterval(0);
    connect(&_rebindTimer, &QTimer::timeout, this, &XdgPortalGlobalShortcuts::rebind);

    _connection.connect(portalService, portalPath, shortcutsInterface, QStringLiteral("Activated"),
                        this, SLOT(onActivated(QDBusMessage)));
    _connection.connect(portalService, portalPath, shortcutsInterface, QStringLiteral("ShortcutsChanged"),
                        this, SLOT(onShortcutsChanged(QDBusMessage)));
}

XdgPortalGlobalShortcuts::~XdgPortalGlobalShortcuts() {
    closeSession();
    const QString name = _connection.name();
    _connection = QDBusConnection(QString());
    QDBusConnection::disconnectFromBus(name);
}

void XdgPortalGlobalShortcuts::setShortcuts(const QList<Shortcut> &shortcuts) {
    _shortcuts = shortcuts;
    _rebindTimer.start();
}

QString XdgPortalGlobalShortcuts::triggerDescription(const QString &id) const {
    return _triggerDescriptions.value(id);
}

bool XdgPortalGlobalShortcuts::canConfigure() const {
    return _version >= 2;
}

void XdgPortalGlobalShortcuts::configure() {
    if (!canConfigure() || _sessionHandle.isEmpty()) {
        return;
    }

    auto message = QDBusMessage::createMethodCall(portalService, portalPath, shortcutsInterface,
                                                  QStringLiteral("ConfigureShortcuts"));
    message << QVariant::fromValue(QDBusObjectPath(_sessionHandle)) << QString() << QVariantMap();
    _connection.asyncCall(message);
}

void XdgPortalGlobalShortcuts::rebind() {
    if (_requestPending) {
        _rebindPending = true;
        return;
    }

    // The portal allows to bind the shortcuts only once per session, a new session is needed to update them.
    closeSession();
    createSession();
}

void XdgPortalGlobalShortcuts::createSession() {
    QVariantMap options;
    options.insert(QStringLiteral("session_handle_token"), newToken());

    _requestPending = true;
    callWithRequest(QStringLiteral("CreateSession"), {}, options, [this](uint response, const QVariantMap &results) {
        _requestPending = false;

        if (response != 0) {
            qWarning() << "Unable to create global shortcuts portal session, response:" << response;
        } else {
            const QVariant handle = results.value(QStringLiteral("session_handle"));
            _sessionHandle = handle.canConvert<QDBusObjectPath>()
                             ? handle.value<QDBusObjectPath>().path()
                             : handle.toString();
        }

        if (_rebindPending) {
            _rebindPending = false;
            rebind();
        } else if (!_sessionHandle.isEmpty()) {
            bindShortcuts();
        }
    });
}

void XdgPortalGlobalShortcuts::bindShortcuts() {
    PortalShortcutList portalShortcuts;
    for (const auto &shortcut: _shortcuts) {
        QVariantMap properties;
        properties.insert(QStringLiteral("description"), shortcut.description);

        const QString trigger = toPortalTrigger(shortcut.preferredTrigger);
        if (!trigger.isEmpty()) {
            properties.insert(QStringLiteral("preferred_trigger"), trigger);
        }
        portalShortcuts << PortalShortcut{shortcut.id, properties};
    }

    const QList<QVariant> args = {
            QVariant::fromValue(QDBusObjectPath(_sessionHandle)),
            QVariant::fromValue(portalShortcuts),
            QString() // parent window
    };

    _requestPending = true;
    callWithRequest(QStringLiteral("BindShortcuts"), args, {}, [this](uint response, const QVariantMap &results) {
        _requestPending = false;

        if (response != 0) {
            qWarning() << "Global shortcuts were not bound by the XDG portal, response:" << response;
        } else {
            updateTriggers(results.value(QStringLiteral("shortcuts")));
        }

        if (_rebindPending) {
            _rebindPending = false;
            rebind();
        }
    });
}

void XdgPortalGlobalShortcuts::closeSession() {
    if (_sessionHandle.isEmpty()) {
        return;
    }

    auto message = QDBusMessage::createMethodCall(portalService, _sessionHandle, sessionInterface,
                                                  QStringLiteral("Close"));
    _connection.asyncCall(message);
    _sessionHandle.clear();
}

void XdgPortalGlobalShortcuts::callWithRequest(const QString &method, QList<QVariant> args, QVariantMap options,
                                               const ResponseHandler &handler) {
    const QString token = newToken();
    options.insert(QStringLiteral("handle_token"), token);

    // Listen on the predictable request path before calling the method to avoid a race with the Response signal
    QString sender = _connection.baseService().mid(1);
    sender.replace(QLatin1Char('.'), QLatin1Char('_'));
    const QString expectedPath = QStringLiteral("/org/freedesktop/portal/desktop/request/%1/%2").arg(sender, token);

    auto *request = new PortalRequest(_connection, handler, this);
    request->listen(expectedPath);

    auto message = QDBusMessage::createMethodCall(portalService, portalPath, shortcutsInterface, method);
    args << options;
    message.setArguments(args);

    auto *watcher = new QDBusPendingCallWatcher(_connection.asyncCall(message), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, request, [request](QDBusPendingCallWatcher *call) {
        call->deleteLater();

        const QDBusPendingReply<QDBusObjectPath> reply = *call;
        if (reply.isError()) {
            qWarning() << "XDG portal call failed:" << reply.error().message();
            request->finish(2, {});
            return;
        }

        // Older portals may use a different request path
        const QString path = reply.value().path();
        if (path != request->path()) {
            request->listen(path);
        }
    });
}

void XdgPortalGlobalShortcuts::updateTriggers(const QVariant &shortcuts) {
    if (!shortcuts.canConvert<QDBusArgument>()) {
        return;
    }

    PortalShortcutList list;
    shortcuts.value<QDBusArgument>() >> list;

    _triggerDescriptions.clear();
    for (const auto &shortcut: list) {
        _triggerDescriptions.insert(shortcut.id,
                                    shortcut.properties.value(QStringLiteral("trigger_description")).toString());
    }
    emit triggersChanged();
}

void XdgPortalGlobalShortcuts::onActivated(const QDBusMessage &message) {
    const QList<QVariant> args = message.arguments();
    if (args.size() < 2 || args[0].value<QDBusObjectPath>().path() != _sessionHandle) {
        return;
    }
    emit activated(args[1].toString());
}

void XdgPortalGlobalShortcuts::onShortcutsChanged(const QDBusMessage &message) {
    const QList<QVariant> args = message.arguments();
    if (args.size() < 2 || args[0].value<QDBusObjectPath>().path() != _sessionHandle) {
        return;
    }
    updateTriggers(args[1]);
}

QString XdgPortalGlobalShortcuts::toPortalTrigger(const QKeySequence &sequence) {
    if (sequence.isEmpty()) {
        return {};
    }

    const QKeyCombination combination = sequence[0];
    const Qt::KeyboardModifiers mods = combination.keyboardModifiers();

    QStringList parts;
    if (mods & Qt::ControlModifier) {
        parts << QStringLiteral("CTRL");
    }
    if (mods & Qt::AltModifier) {
        parts << QStringLiteral("ALT");
    }
    if (mods & Qt::ShiftModifier) {
        parts << QStringLiteral("SHIFT");
    }
    if (mods & Qt::MetaModifier) {
        parts << QStringLiteral("LOGO");
    }
    parts << keyName(combination.key());
    return parts.join(QLatin1Char('+'));
}

#include "XdgPortalGlobalShortcuts.moc"
