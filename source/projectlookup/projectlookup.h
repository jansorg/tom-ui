#ifndef TOM_UI_PROJECTLOOKUP_H
#define TOM_UI_PROJECTLOOKUP_H

#include <QtWidgets/QDialog>

#include "source/gotime/TomControl.h"
#include "source/gotime/ProjectStatusManager.h"
#include "ui_projectLookup.h"

class ProjectLookup : public QDialog, private Ui::ProjectLookupDialog {
Q_OBJECT

public:
    static void show(TomControl *control, QWidget *parent);

protected:
    explicit ProjectLookup(TomControl *control, QWidget *parent);

private:
    TomControl *_control;
};


#endif //TOM_UI_PROJECTLOOKUP_H
