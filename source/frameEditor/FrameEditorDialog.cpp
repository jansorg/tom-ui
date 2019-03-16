#include "FrameEditorDialog.h"

#include <QDateTimeEdit>

FrameEditorDialog::FrameEditorDialog(const Frame &frame, TomControl *control, ProjectStatusManager* statusManager, QWidget *parent) : QDialog(parent), Ui::FrameDialog(),
                                                                                                 _frame(frame),
                                                                                                 _control(control),
                                                                                                 _statusManager(statusManager){

    setupUi(this);
    loadFrame(frame);

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &FrameEditorDialog::saveFrame);
}

void FrameEditorDialog::show(const Frame &frame, TomControl *control, ProjectStatusManager* statusManager, QWidget *parent) {
    auto *dialog = new FrameEditorDialog(frame, control, statusManager, parent);
    dialog->showNormal();
}

void FrameEditorDialog::loadFrame(const Frame &frame) {
    _projectBox->setup(_control, _statusManager);
    _projectBox->setSelectedProject(frame.projectID);

    QString dateTimeFormat = QLocale::system().dateTimeFormat(QLocale::ShortFormat);
    // add secconds and milliseconds to this, Qt isn't offering any default feature for this
    if (dateTimeFormat.endsWith("mm")) {
        dateTimeFormat += ":ss";
    }
    if (dateTimeFormat.endsWith("ss")) {
        dateTimeFormat += ".zzz";
    }

    _beginEdit->setDateTime(frame.startTime);
    _beginEdit->setDisplayFormat(dateTimeFormat);

    _endEdit->setDateTime(frame.stopTime);
    _endEdit->setDisplayFormat(dateTimeFormat);

    _archivedCheckBox->setChecked(frame.archived);
}

void FrameEditorDialog::saveFrame() {
    const QString &projectID = _projectBox->selectedProject().getID();

    _control->updateFrame(QStringList() << _frame.id, QStringList() << projectID,
            true, _beginEdit->dateTime(),
            true, _endEdit->dateTime(),
            true, _notesEdit->toPlainText(),
            true, projectID,
            true, _archivedCheckBox->isChecked());
}
