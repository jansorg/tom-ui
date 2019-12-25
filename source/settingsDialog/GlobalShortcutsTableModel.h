#ifndef TOM_UI_GLOBALSHORTCUTSTABLEMODEL_H
#define TOM_UI_GLOBALSHORTCUTSTABLEMODEL_H

#include <source/GlobalShortcuts.h>
#include "AbstractShortcutTableModel.h"

class GlobalShortcutsTableModel : public AbstractShortcutTableModel {
Q_OBJECT
public:
    explicit GlobalShortcutsTableModel(GlobalShortcuts *shortcuts, QObject *parent);

    QKeySequence getActionKeySequence(const QModelIndex &index) override;

    void updateKeySequence(const QModelIndex &index, const QKeySequence &keySequence) override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private:
    GlobalShortcuts *_shortcuts;
};


#endif //TOM_UI_GLOBALSHORTCUTSTABLEMODEL_H
