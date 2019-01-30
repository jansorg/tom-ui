#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>

#include "source/model/ProjectTreeModel.h"
#include "source/model/UserRoles.h"
#include "projectlookup.h"
#include "projectlistmodel.h"

ProjectLookup::ProjectLookup(TomControl *control, QWidget *parent) : QDialog(parent), _control(control) {
    if (!parent) {
        setWindowFlags(Qt::Window);
    }

    setupUi(this);

    auto *model = new ProjectListModel(control, this);

    auto *completer = new QCompleter(model, this);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
    completer->setMaxVisibleItems(15);
    completer->setCompletionRole(Qt::DisplayRole);
    completer->setFilterMode(Qt::MatchContains);

    projectNameEdit->setCompleter(completer);

    connect(completer, QOverload<const QModelIndex &>::of(&QCompleter::activated), this, &ProjectLookup::onProjectStart);
}

void ProjectLookup::show(TomControl *control, QWidget *parent) {
    auto *dialog = new ProjectLookup(control, parent);
    dialog->showNormal();
}

void ProjectLookup::onProjectStart(const QModelIndex &index) {
    const QString &id = index.data(UserRoles::IDRole).toString();
    const QString &name = index.data(Qt::DisplayRole).toString();
    qDebug() << "project to start" << name << id;

    bool ok = _control->startProject(_control->cachedProject(id));
    if (ok) {
        close();
    } else {
        QMessageBox::warning(parentWidget(), "Error", QString("Failed to start project %1.").arg(name));
        projectNameEdit->clear();
        projectNameEdit->setFocus();
    }
}
