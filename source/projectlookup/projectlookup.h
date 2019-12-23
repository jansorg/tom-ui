#ifndef TOM_UI_PROJECTLOOKUP_H
#define TOM_UI_PROJECTLOOKUP_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QMainWindow>
#include <source/main_window.h>

#include "source/gotime/TomControl.h"
#include "source/gotime/ProjectStatusManager.h"
#include "ui_projectLookup.h"

class ProjectLookup : public QDialog, private Ui::ProjectLookupDialog {
Q_OBJECT

public:
    static void show(TomControl *control, MainWindow *window, QWidget *parent);

    void done(int i) override;

protected:
    explicit ProjectLookup(TomControl *control, MainWindow *window, QWidget *parent);

    void readSettings();

    void writeSettings();

private:
    TomControl *_control;
};


#endif //TOM_UI_PROJECTLOOKUP_H
