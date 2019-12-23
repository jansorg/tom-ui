#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QCheckBox>
#include "TriStateFlagComboBox.h"

TriStateFlagComboBox::TriStateFlagComboBox(QWidget *parent) : QWidget(parent) {
    _box = new QComboBox(this);
    _box->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    connect(_box, QOverload<int>::of(&QComboBox::activated), this, &TriStateFlagComboBox::refresh);

    _inheritedValueLabel = new QLabel(this);
    _inheritedValueLabel->setVisible(false);
    _inheritedValueLabel->setEnabled(false);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(_box);
    layout->addWidget(_inheritedValueLabel);
    setLayout(layout);
}

void TriStateFlagComboBox::initItems(const QString &undefinedLabel,
                                     const QString &trueLabel,
                                     const QString &falseLabel) {
    _box->addItem(undefinedLabel);
    _box->addItem(trueLabel);
    _box->addItem(falseLabel);

    refresh();
}

void TriStateFlagComboBox::setState(TriState value) {
    switch (value) {
        case UNDEFINED:
            _box->setCurrentIndex(0);
            break;
        case TRUE:
            _box->setCurrentIndex(1);
            break;
        case FALSE:
            _box->setCurrentIndex(2);
            break;
    }

    refresh();
}

void TriStateFlagComboBox::setInheritedValue(const QString &value) {
    _inheritedValue = value;

    refresh();
}

TriState TriStateFlagComboBox::getState() {
    int i = _box->currentIndex();
    switch (i) {
        case 0:
            return UNDEFINED;
        case 1:
            return TRUE;
        case 2:
        default:
            return FALSE;
    }
}

void TriStateFlagComboBox::refresh() {
    bool show = !_inheritedValue.isEmpty() && _box->currentIndex() == 0;
    _inheritedValueLabel->setVisible(show);
    _inheritedValueLabel->setText(_inheritedValue);
}
