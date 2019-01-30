#ifndef TOM_UI_PROJECTSTATUSLABEL_H
#define TOM_UI_PROJECTSTATUSLABEL_H

#include <QtWidgets/QLabel>

#include "source/gotime/TomControl.h"

class ProjectStatusLabel : public QLabel {
Q_OBJECT

public:
    explicit ProjectStatusLabel(QWidget *parent);

    void setup(TomControl *control);

private:
    void updateStatus();

    TomControl *_control;
};


#endif //TOM_UI_PROJECTSTATUSLABEL_H
