#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStyle>

#include "source/gotime/StartStopProjectAction.h"
#include "source/icons.h"
#include "source/model/ProjectTreeModel.h"
#include "source/model/UserRoles.h"
#include "projectlookup.h"
#include "projectlistmodel.h"

ProjectLookup::ProjectLookup(TomControl *control, QWidget *parent) : QDialog(parent), _control(control) {
    setWindowFlags(Qt::Tool);

    if (!parent) {
//        setAttribute(Qt::WA_AlwaysStackOnTop);
//        setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    }

    setupUi(this);
    if (!parent) {
        layout()->setContentsMargins(5, 5, 5, 5);
        projectNameEdit->setStyleSheet("* { padding: 3px; }");
    }

    activeProjectLabel->setup(_control);

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

    projectNameEdit->setPlaceholderText(tr("Enter project name or press %1 for recent projects").arg(tr("down arrow")));

    // setup status of the active project if there's one
    stopProjectButton->setIcon(Icons::projectStop());
    const auto &active = _control->cachedStatus();
    if (active.isValid) {
        connect(stopProjectButton, &QPushButton::clicked, [this] {
            _control->stopActivity();
            close();
        });
    } else {
        activeProjectStatus->setEnabled(false);
    }
}

void ProjectLookup::keyPressEvent(QKeyEvent *event) {
    // display a menu of the most recent proejcts on arrow down
    qDebug() << "keyPressEvent" << event;
    if (event->key() == Qt::Key_Down) {
        event->accept();

        QMenu menu(tr("Recent projects"), this);
        const QList<Project> &projects = _control->loadRecentProjects();
        if (projects.isEmpty()) {
            menu.addSection(tr("No recent projects available"));
        } else {
            menu.addSection(tr("Recent projects"));
            for (const auto &project:projects) {
                auto *action = new StartProjectAction(project, _control, &menu);
                connect(action, &QAction::triggered, [this] { close(); });
                menu.addAction(action);
            }
        }

        const QRect &rect = projectNameEdit->rect();
        menu.setFixedWidth(rect.width());
        menu.exec(projectNameEdit->mapToGlobal(rect.bottomLeft()));
        return;
    }

    QDialog::keyPressEvent(event);
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
