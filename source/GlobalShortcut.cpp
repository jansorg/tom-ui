#include "GlobalShortcut.h"

#include <utility>

GlobalShortcut::GlobalShortcut(QString id, QString label, const QKeySequence &shortcut, QObject *parent)
        : QObject(parent),
          _id(std::move(id)),
          _label(std::move(label)),
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
    if (_globalShortcut == nullptr) {
        return QKeySequence();
    }
    return _globalShortcut->shortcut();
}

bool GlobalShortcut::setShortcut(const QKeySequence &shortcut) {
    if (shortcut.isEmpty()) {
        if (_globalShortcut != nullptr) {
            delete _globalShortcut;
            _globalShortcut = nullptr;
        }
        return true;
    }

    if (_globalShortcut == nullptr) {
        _globalShortcut = new QxtGlobalShortcut(shortcut, this);
        connect(_globalShortcut, &QxtGlobalShortcut::activated, this, &GlobalShortcut::activated);
    } else {
        _globalShortcut->setShortcut(shortcut);
    }
    return true;
}

GlobalShortcut::~GlobalShortcut() = default;
