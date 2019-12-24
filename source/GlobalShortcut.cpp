#include "GlobalShortcut.h"

#include <utility>

GlobalShortcut::GlobalShortcut(QString id, QString label, const QKeySequence &shortcut, QObject *parent)
        : QxtGlobalShortcut(shortcut, parent),
          _id(std::move(id)),
          _label(std::move(label)) {

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

GlobalShortcut::~GlobalShortcut() = default;
