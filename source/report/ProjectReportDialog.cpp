#include <utility>
#include <QtGlobal>
#include <source/main_window.h>

#include "ProjectReportDialog.h"

#include "source/model/ProjectTreeModel.h"
#include "source/view/ProjectTreeView.h"
#include "source/model/UserRoles.h"

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
//    _webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
//    _webView->settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    previewFrame->layout()->addWidget(_webView);
    _webView->show();
#else
    previewFrame->hide();
#endif

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

    // read before the signals are connected to avoid updates on state changes
    readSettings();

    connect(updateButton, &QPushButton::pressed, this, &ProjectReportDialog::updateReport);

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

    QTimer::singleShot(500, this, &ProjectReportDialog::updateReport);
}

void ProjectReportDialog::updateReport() {
    QStringList splits = _splitModel->checkedItems();

    int frameRoundingMin = frameRoundingValue->value();

    const QString &frameModeText = frameRoundingMode->currentText();
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

    _projects.clear();
    Project project = projectsBox->selectedProject();
    if (project.isValid()) {
        _projects << project.getID();
    }

    QString html = _control->htmlReport(_tempFile, _projects,
                                        subprojectsCheckbox->isChecked(),
                                        start, end, frameMode, frameRoundingMin,
                                        splits, templateBox->currentText(),
                                        matrixTablesCheckbox->isChecked(),
                                        showEmptyCheckbox->isChecked(),
                                        showSummaryCheckbox->isChecked(),
                                        includeArchivedCheckBox->isChecked(),
                                        titleEdit->text(), descriptionEdit->toPlainText(),
                                        showSalesCheckbox->isChecked());

#ifdef TOM_REPORTS
    if (QFile::exists(_tempFile)) {
        _webView->load(QUrl::fromLocalFile(_tempFile));
    } else {
        _webView->setHtml(html);
    }
#endif
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

    const QVariant &split = settings.value("reportDialog/splitValues");
    if (split.isValid()) {
        _splitModel->setCheckedItems(split.toStringList());
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
    settings.setValue("reportDialog/splitValues", _splitModel->checkedItems());

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

