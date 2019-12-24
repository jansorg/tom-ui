#ifndef TOM_UI_GLOBALSHORTCUT_H
#define TOM_UI_GLOBALSHORTCUT_H

#include <qxt/qxtglobalshortcut.h>
#include <QtCore/QSettings>

class GlobalShortcut : public QxtGlobalShortcut {
public:
    explicit GlobalShortcut(QString id, QString label, const QKeySequence &shortcut, QObject *parent = nullptr);

    ~GlobalShortcut() override;

    const QString &id() const;

    const QString &label() const;

    void load(const QSettings &settings);

    void save(QSettings &settings);

private:
    QString key() const;

    QString _id;
    QString _label;
};

#endif //TOM_UI_GLOBALSHORTCUT_H
