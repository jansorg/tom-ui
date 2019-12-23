#ifndef TOM_UI_TRISTATECOMBOBOX_H
#define TOM_UI_TRISTATECOMBOBOX_H

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "data/TriState.h"

class TriStateFlagComboBox : public QWidget {
Q_OBJECT
public:
    explicit TriStateFlagComboBox(QWidget *parent);

    void initItems(const QString &undefinedLabel = tr("Inherit"),
                   const QString &trueLabel = tr("Enabled"),
                   const QString &falseLabel = tr("Disabled"));

    void setState(TriState value);

    TriState getState();

    void setInheritedValue(const QString& state);

private slots:
    void refresh();

private:

    QComboBox *_box;
    QLabel *_inheritedValueLabel;
    QString _inheritedValue;
};

#endif //TOM_UI_TRISTATECOMBOBOX_H
