#ifndef TOM_UI_ACTIONSHORTCUTTABLE_H
#define TOM_UI_ACTIONSHORTCUTTABLE_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QPushButton>

#include "AbstractShortcutTableModel.h"

class ActionShortcutTable : public QWidget {
Q_OBJECT
public:
    explicit ActionShortcutTable(QWidget *parent = nullptr);

    void setModel(AbstractShortcutTableModel *model);

private slots:

    void selectedRowChanged(const QModelIndex &row);

    void keySequenceChanged();

    void clearKeySequence();

private:
    AbstractShortcutTableModel *_model;
    QTableView *_actionsTable;
    QKeySequenceEdit *_keySequenceEdit;
    QPushButton *_resetButton;
};


#endif //TOM_UI_ACTIONSHORTCUTTABLE_H
