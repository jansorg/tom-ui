#include "GlobalShortcut.h"

#include <utility>
#include <QDebug>

GlobalShortcut::GlobalShortcut(QString id, QString label, const QKeySequence &shortcut, bool useNativeBackend,
                               QObject *parent)
        : QObject(parent),
          _id(std::move(id)),
          _label(std::move(label)),
          _useNativeBackend(useNativeBackend),
          _globalShortcut(nullptr) {

    // register initial shortcut
    setShortcut(shortcut);
}

const QString &GlobalShortcut::id() const {
    return _id;
}

const QString &GlobalShortcut::label() const {
    return _label;
}

void GlobalShortcut::load(const QSettings &settings) {
    const QVariant &variant = settings.value(key());
    if (variant.isValid() && variant.canConvert<QKeySequence>()) {
        setShortcut(variant.value<QKeySequence>());
    }
}

void GlobalShortcut::save(QSettings &settings) {
    settings.setValue(key(), shortcut());
}

QString GlobalShortcut::key() const {
    return QString("globalShortcuts/%1").arg(_id);
}

QKeySequence GlobalShortcut::shortcut() const {
    if (_globalShortcut != nullptr) {
        return _globalShortcut->shortcut();
    }
    return _shortcut;
}

bool GlobalShortcut::setShortcut(const QKeySequence &shortcut) {
    // only the first key combination is supported by global shortcuts
    const QKeySequence newShortcut = shortcut.isEmpty() ? QKeySequence() : QKeySequence(shortcut[0]);
    const bool changed = newShortcut != _shortcut;
    _shortcut = newShortcut;

    bool ok = true;
    if (!_useNativeBackend) {
        // registered by the owner, e.g. via the XDG desktop portal
    } else if (newShortcut.isEmpty()) {
        delete _globalShortcut;
        _globalShortcut = nullptr;
    } else if (_globalShortcut == nullptr) {
        _globalShortcut = new QxtGlobalShortcut(newShortcut, this);
        connect(_globalShortcut, &QxtGlobalShortcut::activated, this, &GlobalShortcut::activated);
        ok = _globalShortcut->isValid();
    } else {
        ok = _globalShortcut->setShortcut(newShortcut);
    }

    if (!ok) {
        qWarning() << "Unable to register global shortcut" << newShortcut.toString() << "for" << _id;
    }

    if (changed) {
        emit shortcutChanged();
    }
    return ok;
}

GlobalShortcut::~GlobalShortcut() = default;
