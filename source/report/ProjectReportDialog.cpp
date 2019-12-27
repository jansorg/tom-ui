#include <utility>
#include <QtGlobal>
#include <source/main_window.h>
#include <source/commonModels/TranslatedStringlistModel.h>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>
#include <QCompleter>

#include "ProjectReportDialog.h"

#include "source/model/ProjectTreeModel.h"
#include "source/view/ProjectTreeView.h"
#include "source/model/UserRoles.h"
#include "source/commonModels/FileSystemModel.h"

ProjectReportDialog::ProjectReportDialog(const QList<Project> &projects, TomControl *control,
                                         ProjectStatusManager *statusManager, QWidget *parent) : QDialog(parent),
                                                                                                 _projects(),
                                                                                                 _control(control),
                                                                                                 _splitModel(new ReportSplitModel(this)),
                                                                                                 _tempDir("tom-report") {

    setAttribute(Qt::WA_DeleteOnClose);

    for (const auto &p : projects) {
        if (p.isValid()) {
            _projects << p.getID();
        }
    }

    setupUi(this);
#ifdef TOM_REPORTS
    _webView = new QWebEngineView(this);
    _webView->setContextMenuPolicy(Qt::NoContextMenu);
//    _webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
//    _webView->settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    previewFrame->layout()->addWidget(_webView);
    _webView->show();
#else
    previewFrame->hide();
#endif

    // fix up widgets
    auto *completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    auto *fsModel = new FileSystemModel(completer);
    fsModel->setRootPath("");
    completer->setModel(fsModel);
    cssFileEdit->setCompleter(completer);

    if (_tempDir.isValid()) {
        _tempFile = _tempDir.filePath("report.html");
    }

    projectsBox->setup(control, statusManager);

    // split list
    splitMoveUp->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    splitMoveDown->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    connect(splitMoveUp, &QPushButton::pressed, [this] { moveSplitSelection(-1); });
    connect(splitMoveDown, &QPushButton::pressed, [this] { moveSplitSelection(1); });
    _splitModel->setCheckedItems(QStringList() << "project");
    splitList->setModel(_splitModel);
    splitList->setSelectionMode(QAbstractItemView::SingleSelection);
    splitList->adjustSize();

    dateStart->setDate(QDateTime::currentDateTime().date());
    dateEnd->setDate(QDateTime::currentDateTime().date());

    templateBox->setModel(new TranslatedStringlistModel(
            QStringList() << "default" << "timelog",
            QStringList() << tr("Default") << tr("Time Entries"),
            this
    ));

    frameRoundingMode->setModel(new TranslatedStringlistModel(
            QStringList() << "up" << "up or down",
            QStringList() << tr("up") << tr("up or down"),
            this
    ));

    // read before the signals are connected to avoid updates on state changes
    readSettings();

    // setup actions buttons
    auto *updateAction = new QAction("&Update report", this);
    connect(updateAction, &QAction::triggered, this, &ProjectReportDialog::updateReport);
    updateAction->setShortcuts(QKeySequence::Refresh);
    updateAction->setShortcutVisibleInContextMenu(true);

    auto saveHTMLAction = new QAction("&Save report as HTML", this);
    connect(saveHTMLAction, &QAction::triggered, this, &ProjectReportDialog::saveReportHTML);
    saveHTMLAction->setShortcuts(QKeySequence::Save);
    saveHTMLAction->setShortcutVisibleInContextMenu(true);

    auto actionsMenu = new QMenu("Actions", this);
    actionsMenu->addAction(updateAction);
    actionsMenu->addAction(saveHTMLAction);

    actionsButton->setDefaultAction(updateAction);
    actionsButton->setMenu(actionsMenu);

    // connections
    connect(projectsBox, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);
    connect(subprojectsCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);

    connect(includeArchivedCheckBox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(dateFilterCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(dateStart, &QDateEdit::dateChanged, this, &ProjectReportDialog::updateReport);
    connect(dateEnd, &QDateEdit::dateChanged, this, &ProjectReportDialog::updateReport);

    connect(roundEntriesCheckBox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(frameRoundingMode, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);
    connect(frameRoundingValue, QOverload<int>::of(&QSpinBox::valueChanged), this, &ProjectReportDialog::updateReport);

    connect(templateBox, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);

    connect(_splitModel, &ReportSplitModel::itemStateChanged, this, &ProjectReportDialog::updateReport);
    connect(_splitModel, &ReportSplitModel::dataChanged, this, &ProjectReportDialog::updateReport);
    connect(_splitModel, &ReportSplitModel::modelReset, this, &ProjectReportDialog::updateReport);

    connect(matrixTablesCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showEmptyCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showSummaryCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showSalesCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showTrackedCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showUntrackedCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);

    QTimer::singleShot(500, this, &ProjectReportDialog::updateReport);
}

void ProjectReportDialog::updateReport() {
    _projects.clear();
    Project project = projectsBox->selectedProject();
    if (project.isValid()) {
        _projects << project.getID();
    }

    QString html = reportHTML(_tempFile);

#ifdef TOM_REPORTS
    if (QFile::exists(_tempFile)) {
        _webView->load(QUrl::fromLocalFile(_tempFile));
    } else {
        _webView->setHtml(html);
    }
#endif
}

void ProjectReportDialog::saveReportHTML() {
    _projects.clear();
    Project project = projectsBox->selectedProject();
    if (project.isValid()) {
        _projects << project.getID();
    }

    QDateTime current(QDateTime::currentDateTime());
    QString defaultFile = QString("tom-report.%1.html").arg(current.toString(Qt::ISODate));

    const QString &fileName = QFileDialog::getSaveFileName(this, tr("Save Report as HTML"), defaultFile, tr("HTML files (*.html *.htm);;All files (*)"));
    if (fileName != "") {
        reportHTML(fileName);
    }
}

void ProjectReportDialog::moveSplitSelection(int delta) {
    const QModelIndex &selected = splitList->selectionModel()->currentIndex();
    if (selected.isValid()) {
        int row = selected.row();
        splitList->clearSelection();
        if (_splitModel->moveRow(QModelIndex(), row, QModelIndex(), row + delta)) {
            const QModelIndex &newRow = selected.siblingAtRow(row + delta);
            splitList->selectionModel()->setCurrentIndex(newRow, QItemSelectionModel::SelectCurrent);
        }
    }
}

void ProjectReportDialog::projectIndexSelected(const QModelIndex &index) {
    auto id = index.data(UserRoles::IDRole).toString();

    _projects = QStringList() << id;
    updateReport();
}

void ProjectReportDialog::done(int i) {
    writeSettings();
    QDialog::done(i);
}

void ProjectReportDialog::readSettings() {
    QSettings settings;

    const QVariant &geometry = settings.value("reportDialog/geometry");
    if (geometry.isValid()) {
        restoreGeometry(geometry.toByteArray());
    }

    const QVariant &splitData = settings.value("reportDialog/splitValues/data");
    if (splitData.isValid()) {
        _splitModel->setStringList(splitData.toStringList());
    }

    const QVariant &splitChecked = settings.value("reportDialog/splitValues/checked");
    if (splitChecked.isValid()) {
        _splitModel->setCheckedItems(splitChecked.toStringList());
    }

    readSettings(settings, this);
}

void ProjectReportDialog::readSettings(QSettings &settings, QObject *child) {
    bool stop = false;
    if (child->isWidgetType() && !child->objectName().isEmpty()) {
        const QString &key = QString("reportDialog/state/%1").arg(child->objectName());

        const QVariant &value = settings.value(key);
        if (value.isValid()) {
            stop = true;
            if (auto *box = qobject_cast<QCheckBox *>(child)) {
                box->setChecked(value.toBool());
            } else if (auto *lineEdit = qobject_cast<QLineEdit *>(child)) {
                lineEdit->setText(value.toString());
            } else if (auto *textInput = qobject_cast<QPlainTextEdit *>(child)) {
                textInput->setPlainText(value.toString());
            } else if (auto *projects = qobject_cast<ProjectTreeComboBox *>(child)) {
                projects->setSelectedProject(value.toString());
            } else if (auto *combo = qobject_cast<QComboBox *>(child)) {
                combo->setCurrentText(value.toString());
            } else if (auto *spinBox = qobject_cast<QSpinBox *>(child)) {
                spinBox->setValue(value.toInt());
            } else if (auto *dateEdit = qobject_cast<QDateEdit *>(child)) {
                dateEdit->setDate(value.toDate());
            } else {
                stop = false;
            }
        }
    }

    if (!stop) {
        for (auto *c : child->children()) {
            readSettings(settings, c);
        }
    }
}

void ProjectReportDialog::writeSettings() {
    QSettings settings;

    settings.setValue("reportDialog/geometry", saveGeometry());
    settings.setValue("reportDialog/splitValues/data", _splitModel->stringList());
    settings.setValue("reportDialog/splitValues/checked", _splitModel->checkedItems());

    writeSettings(settings, this);
}

void ProjectReportDialog::writeSettings(QSettings &settings, QObject *child) {
    bool stop = false;

    if (child->isWidgetType() && !child->objectName().isEmpty()) {
        stop = true;
        const QString &key = QString("reportDialog/state/%1").arg(child->objectName());
        if (auto *box = qobject_cast<QCheckBox *>(child)) {
            settings.setValue(key, box->isChecked());
        } else if (auto *lineEdit = qobject_cast<QLineEdit *>(child)) {
            settings.setValue(key, lineEdit->text());
        } else if (auto *textInput = qobject_cast<QPlainTextEdit *>(child)) {
            settings.setValue(key, textInput->toPlainText());
        } else if (auto *projects = qobject_cast<ProjectTreeComboBox *>(child)) {
            settings.setValue(key, projects->selectedProject().getID());
        } else if (auto *combo = qobject_cast<QComboBox *>(child)) {
            settings.setValue(key, combo->currentText());
        } else if (auto *spinBox = qobject_cast<QSpinBox *>(child)) {
            settings.setValue(key, spinBox->value());
        } else if (auto *dateEdit = qobject_cast<QDateEdit *>(child)) {
            settings.setValue(key, dateEdit->date());
        } else {
            stop = false;
        }
    }

    if (!stop) {
        for (auto *c : child->children()) {
            writeSettings(settings, c);
        }
    }
}

QString ProjectReportDialog::reportHTML(const QString &filename) const {
    QStringList splits = _splitModel->checkedItems();

    int frameRoundingMin = frameRoundingValue->value();

    const QString &frameModeText = frameRoundingMode->currentData(Qt::EditRole).toString();
    TimeRoundingMode frameMode = NONE;
    if (roundEntriesCheckBox->isChecked()) {
        if (frameModeText == "up") {
            frameMode = UP;
        } else if (frameModeText == "down") {
            frameMode = DOWN;
        } else if (frameModeText == "up or down") {
            frameMode = NEAREST;
        }
    }

    QDate start;
    QDate end;
    if (dateFilterCheckbox->isChecked()) {
        start = dateStart->date();
        end = dateEnd->date();
    }

    const QString &templateId = templateBox->currentData(Qt::EditRole).toString();

    return _control->htmlReport(filename, _projects,
                                subprojectsCheckbox->isChecked(),
                                start, end, frameMode, frameRoundingMin,
                                splits,
                                templateId,
                                matrixTablesCheckbox->isChecked(),
                                showEmptyCheckbox->isChecked(),
                                showSummaryCheckbox->isChecked(),
                                includeArchivedCheckBox->isChecked(),
                                titleEdit->text(), descriptionEdit->toPlainText(),
                                showSalesCheckbox->isChecked(),
                                showTrackedCheckbox->isChecked(),
                                showUntrackedCheckbox->isChecked(),
                                cssFileEdit->text());
}

