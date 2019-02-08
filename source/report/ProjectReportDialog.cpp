#include <utility>
#include <QtGlobal>

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

    //updateReport();
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
    const QModelIndex &current = projectsBox->view()->currentIndex();
    if (current.isValid()) {
        _projects << current.data(UserRoles::IDRole).toString();
    }

    QString html = _control->htmlReport(_tempFile, _projects,
                                        subprojectsCheckbox->isChecked(),
                                        start, end, frameMode, frameRoundingMin,
                                        splits, templateBox->currentText(),
                                        matrixTablesCheckbox->isChecked(),
                                        showEmptyCheckbox->isChecked(),
                                        showSummaryCheckbox->isChecked(),
                                        includeArchivedCheckBox->isChecked(),
                                        titleEdit->text(), descriptionEdit->toPlainText());

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
    qDebug() << "project selected" << index << id;

    _projects = QStringList() << id;
    updateReport();
}
