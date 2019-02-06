#ifndef TOM_UI_PROJECTEDITORDIALOG_H
#define TOM_UI_PROJECTEDITORDIALOG_H

#include <QtWidgets/QDialog>
#include <source/gotime/ProjectStatusManager.h>

#include "ui_project_dialog.h"
#include "source/gotime/TomControl.h"

class ProjectEditorDialog : public QDialog, private Ui::ProjectDialog {
public:
    ProjectEditorDialog(const Project &project, TomControl *control,ProjectStatusManager* statusManager,  QWidget *parent);

    static void show(const Project &project, TomControl *control, ProjectStatusManager* statusManager, QWidget *parent);

private slots:

    void saveProject();

private:
    TomControl *_control;
    ProjectStatusManager* _statusManager;
    Project _project;

    void loadProject(const Project &project);
};


#endif //TOM_UI_PROJECTEDITORDIALOG_H
