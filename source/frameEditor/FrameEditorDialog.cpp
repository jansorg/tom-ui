#include <QDateTimeEdit>
#include <QPushButton>

#include "FrameEditorDialog.h"

FrameEditorDialog::FrameEditorDialog(const Frame &frame, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) : QDialog(parent), Ui::FrameDialog(),
                                                                                                                                      _frame(frame),
                                                                                                                                      _control(control),
                                                                                                                                      _statusManager(statusManager) {

    setupUi(this);
    setWindowTitle(tr("Edit Time Entry"));
    loadFrame(frame);

    connect(_buttonBox, &QDialogButtonBox::accepted, this, &FrameEditorDialog::saveFrame);
    connect(_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, [this] { loadFrame(_frame); });
}

void FrameEditorDialog::show(const Frame &frame, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent) {
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

    _beginEdit->setDisplayFormat(dateTimeFormat);
    _beginEdit->setDateTime(frame.startTime);

    _endEdit->setDisplayFormat(dateTimeFormat);
    if (frame.stopTime.isValid()) {
        _endEdit->setDateTime(frame.stopTime);
    } else {
        _endEdit->setDateTime(QDateTime::currentDateTime());
    }

    _archivedCheckBox->setChecked(frame.archived);

    _notesEdit->setPlainText(frame.notes);
}

void FrameEditorDialog::saveFrame() {
    const QString &projectID = _projectBox->selectedProject().getID();

    _control->updateFrames(QStringList() << _frame.id, QStringList() << projectID,
                           true, _beginEdit->dateTime(),
                           true, _endEdit->dateTime(),
                           true, _notesEdit->toPlainText(),
                           true, projectID,
                           true, _archivedCheckBox->isChecked());
}
