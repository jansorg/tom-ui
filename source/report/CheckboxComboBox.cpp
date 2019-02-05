#include "CheckboxComboBox.h"

#include <QtCore>
#include <QMouseEvent>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QtWidgets/QTreeView>
#include <source/view/ProjectTreeView.h>

CheckboxComboBox::CheckboxComboBox(QWidget *parent) : QComboBox(parent), _skipNextHide(false) {
}

void CheckboxComboBox::showPopup() {
    QComboBox::showPopup();
}

void CheckboxComboBox::hidePopup() {
//    setCurrentIndex(view()->currentIndex().row());
    if (_skipNextHide) {
        _skipNextHide = false;
    } else {
        QComboBox::hidePopup();
    }
}

bool CheckboxComboBox::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress && watched == view()->viewport()) {
        qDebug() << "event filter";
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        QModelIndex index = view()->indexAt(mouseEvent->pos());
        if (!view()->visualRect(index).contains(mouseEvent->pos())) {
            _skipNextHide = true;
        } else {
            emit indexSelected(index);
        }
    }
    return QComboBox::eventFilter(watched, event);
}

void CheckboxComboBox::setup(ProjectTreeView *view) {
    setView(view);
    view->header()->hide();
    view->viewport()->installEventFilter(this);
}
