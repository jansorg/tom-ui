#ifndef TOM_UI_FRAMEEDITORDIALOG_H
#define TOM_UI_FRAMEEDITORDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_frame_dialog.h"
#include "source/gotime/TomControl.h"
#include "source/gotime/ProjectStatusManager.h"

class FrameEditorDialog : public QDialog, private Ui::FrameDialog {
    Q_OBJECT

public:
    FrameEditorDialog(const Frame &frame, TomControl *control, ProjectStatusManager* statusManager, QWidget *parent);

    static void show(const Frame &frame, TomControl *control, ProjectStatusManager* statusManager, QWidget *parent);

private slots:

    void saveFrame();

private:
    Frame _frame;
    TomControl *_control;
    ProjectStatusManager* _statusManager;

    void loadFrame(const Frame& frame);
};


#endif //TOM_UI_FRAMEEDITORDIALOG_H
