#ifndef TOM_UI_REPORTDIALOG_H
#define TOM_UI_REPORTDIALOG_H

#include "ui_reportDialog.h"
#include "source/data/Project.h"
#include "source/gotime/TomControl.h"

class ProjectReportDialog : public QDialog, private Ui::ReportDialog {
public:
    ProjectReportDialog(QList<Project> projects, TomControl *control, QWidget *parent);

private slots:

    void updateReport();

private:
    QList<Project> _projects;
    TomControl *_control;
};

#endif //TOM_UI_REPORTDIALOG_H
