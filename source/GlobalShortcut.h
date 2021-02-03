#ifndef TOM_UI_GLOBALSHORTCUT_H
#define TOM_UI_GLOBALSHORTCUT_H

#include <qxt/qxtglobalshortcut.h>
#include <QtCore/QSettings>

/**
 * A configurable global shortcut.
 * On macOS, an empty QKeySequence is triggered when "a" is pressed. We work around this by
 * avoiding to create a QxtGlobalShortcut for empty key sequences.
 */
class GlobalShortcut : public QObject {
    Q_OBJECT
    Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)

public:
    explicit GlobalShortcut(QString id, QString label, const QKeySequence &shortcut, QObject *parent = nullptr);

    ~GlobalShortcut() override;

    const QString &id() const;

    const QString &label() const;

    void load(const QSettings &settings);

    void save(QSettings &settings);

    QKeySequence shortcut() const;
    bool setShortcut(const QKeySequence& shortcut);

Q_SIGNALS:
    void activated(QxtGlobalShortcut *self);

private:
    QString key() const;

    QString _id;
    QString _label;
    QxtGlobalShortcut* _globalShortcut;
};

#endif //TOM_UI_GLOBALSHORTCUT_H
