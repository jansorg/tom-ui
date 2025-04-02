#include <QKeyEvent>
#include <QCompleter>
#include <QMenu>
#include <source/model/UserRoles.h>
#include <QMessageBox>

#include "source/gotime/StartStopProjectAction.h"
#include "source/commonModels/projectlistmodel.h"
#include "source/projectlookup/projectlookup.h"
#include "projectcompletionlineedit.h"

ProjectCompletionLineEdit::ProjectCompletionLineEdit(QWidget *parent) : QLineEdit(parent), _control(nullptr) {
    setPlaceholderText(tr("Enter project name or press %1 for recent projects").arg(tr("down arrow")));
}

void ProjectCompletionLineEdit::setup(TomControl *control) {
    setFocusPolicy(Qt::StrongFocus);

    _control = control;

    auto *model = new ProjectListModel(control, this);
    auto *completer = new QCompleter(model, this);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
    completer->setMaxVisibleItems(15);
    completer->setCompletionRole(Qt::DisplayRole);
    completer->setFilterMode(Qt::MatchContains);

    setCompleter(completer);
    connect(completer, QOverload<const QModelIndex &>::of(&QCompleter::activated), this, &ProjectCompletionLineEdit::onProjectSelected);
}

void ProjectCompletionLineEdit::keyPressEvent(QKeyEvent *event) {
    // display a menu of the most recent proejcts on arrow down
    if (event->key() != Qt::Key_Down) {
        QLineEdit::keyPressEvent(event);
        return;
    }

    QMenu menu(tr("Recent projects"), this);
    const QList<Project> &projects = _control->cachedRecentProjects();
    if (projects.isEmpty()) {
        menu.addSection(tr("No recent projects available"));
    } else {
        menu.addSection(tr("Recent projects"));
        for (const auto &project:projects) {
            auto *action = new StartProjectAction(project, _control, &menu);
            connect(action, &QAction::triggered, [this] { emit completionAccepted(); });
            menu.addAction(action);
        }
        // preselect the first project in the list (the section title is also an action)
        menu.setActiveAction(menu.actions().at(1));
    }

    const QRect &r = rect();
    menu.setFixedWidth(r.width());
    menu.exec(mapToGlobal(r.bottomLeft()));
}

void ProjectCompletionLineEdit::onProjectSelected(const QModelIndex &index) {
    const QString &id = index.data(UserRoles::IDRole).toString();
    if (!_control->startProject(_control->cachedProject(id))) {
        //fixme show error
        clear();
        setFocus();
    }

    emit completionAccepted();
}
