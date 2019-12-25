#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>

#include "ActionShortcutTable.h"
#include "icons.h"
#include "ApplicationShortcutsTableModel.h"

ActionShortcutTable::ActionShortcutTable(QWidget *parent) : QWidget(parent) {
    _actionsTable = new QTableView(this);
    _actionsTable->setCornerButtonEnabled(false);
    _actionsTable->verticalHeader()->hide();
    _actionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _actionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _actionsTable->horizontalHeader()->setStretchLastSection(true);

    _keySequenceEdit = new QKeySequenceEdit(this);
    _resetButton = new QPushButton(Icons::keySequenceReset(), tr("Clear shortcut"), this);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(_actionsTable);

    auto *keyEditLayout = new QHBoxLayout(this);
    layout->addLayout(keyEditLayout);
    keyEditLayout->addWidget(_keySequenceEdit);
    keyEditLayout->addWidget(_resetButton);
}

void ActionShortcutTable::setModel(AbstractShortcutTableModel *model) {
    _model = model;
    _actionsTable->setModel(model);

    connect(_actionsTable->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &ActionShortcutTable::selectedRowChanged);
    connect(_keySequenceEdit, &QKeySequenceEdit::editingFinished,
            this, &ActionShortcutTable::keySequenceChanged);
    connect(_resetButton, &QPushButton::clicked,
            this, &ActionShortcutTable::clearKeySequence);

    _actionsTable->resizeColumnsToContents();
}

void ActionShortcutTable::selectedRowChanged(const QModelIndex &row) {
    _keySequenceEdit->clear();

    QKeySequence keySequence = _model->getActionKeySequence(row);
    _keySequenceEdit->setKeySequence(keySequence);
}

void ActionShortcutTable::keySequenceChanged() {
    const QModelIndexList &selection = _actionsTable->selectionModel()->selectedRows(1);
    if (selection.size() != 1){
        return;
    }

    const QModelIndex &index = selection.first();
    if (index.isValid()) {
        _model->updateKeySequence(index, _keySequenceEdit->keySequence());
    }
}

void ActionShortcutTable::clearKeySequence() {
    _keySequenceEdit->clear();
    keySequenceChanged();
}

