#ifndef TOM_UI_ENTRYNOTEEDITOR_H
#define TOM_UI_ENTRYNOTEEDITOR_H

#include <QtWidgets/QDialog>

#include "data/Project.h"

class EntryNoteEditor : public QDialog {
Q_OBJECT
public:
    explicit EntryNoteEditor(const Project &project, const QString &notes, QWidget *parent = nullptr);

    static QString show(const Project &project, const QString &notes, bool *ok = nullptr, QWidget* parent = nullptr);
private:
};

#endif //TOM_UI_ENTRYNOTEEDITOR_H
