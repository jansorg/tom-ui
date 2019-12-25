#ifndef TOM_UI_APPLICATIONSHORTCUTSTABLEMODEL_H
#define TOM_UI_APPLICATIONSHORTCUTSTABLEMODEL_H

#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtWidgets/QAction>

#include "settings/TomSettings.h"
#include "AbstractShortcutTableModel.h"

class ApplicationShortcutsTableModel : public AbstractShortcutTableModel {
Q_OBJECT
public:
    explicit ApplicationShortcutsTableModel(QList<QAction *> actions, TomSettings *settings, QObject *parent);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void updateKeySequence(const QModelIndex &index, const QKeySequence &keySequence) override;

    QKeySequence getActionKeySequence(const QModelIndex &index) override;

private:
    QList<QAction *> _actions;
    TomSettings *_settings;
};


#endif //TOM_UI_APPLICATIONSHORTCUTSTABLEMODEL_H
