#ifndef TOM_UI_CHECKBOXCOMBOBOX_H
#define TOM_UI_CHECKBOXCOMBOBOX_H

#include <QtWidgets/QComboBox>
#include <source/view/ProjectTreeView.h>

class CheckboxComboBox : public QComboBox {
Q_OBJECT
public:
    explicit CheckboxComboBox(QWidget *parent);

    void setup(ProjectTreeView *view);

    bool eventFilter(QObject *watched, QEvent *event) override;

    void showPopup() override;

    void hidePopup() override;

    bool _skipNextHide;

signals:

    void indexSelected(const QModelIndex &index);

};

#endif //TOM_UI_CHECKBOXCOMBOBOX_H
