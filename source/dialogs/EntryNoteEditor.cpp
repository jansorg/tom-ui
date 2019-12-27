#include "EntryNoteEditor.h"

EntryNoteEditor::EntryNoteEditor(const Project &project, const QString &notes, QWidget *parent) : QDialog(parent) {
    setModal(true);
    setWindowTitle(tr("Edit Time Entry of Project %1".arg(project.getShortName())));
}

QString EntryNoteEditor::show(const Project &project, const QString &notes, bool *ok, QWidget *parent) {
    auto *dialog = new EntryNoteEditor(project, notes, parent);
    dialog->showNormal();
}
