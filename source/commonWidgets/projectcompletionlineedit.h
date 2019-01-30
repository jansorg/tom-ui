#ifndef TOM_UI_PROJECTCOMPLETIONLINEEDIT_H
#define TOM_UI_PROJECTCOMPLETIONLINEEDIT_H

#include <QtWidgets/QLineEdit>

#include "source/gotime/TomControl.h"

class ProjectCompletionLineEdit : public QLineEdit {
Q_OBJECT

public:
    explicit ProjectCompletionLineEdit(QWidget *parent);

    void setup(TomControl *control);

signals:

    void completionAccepted();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:

    void onProjectSelected(const QModelIndex &);

private:
    TomControl *_control;
};


#endif //TOM_UI_PROJECTCOMPLETIONLINEEDIT_H
