#include <utility>
#include <QtGlobal>
#include <QWebEngineSettings>

#include "ProjectReportDialog.h"

ProjectReportDialog::ProjectReportDialog(QList<Project> projects, TomControl *control, QWidget *parent)
        : QDialog(parent), _projects(std::move(projects)), _control(control), _splitModel(new ReportSplitModel(this)), _tempDir("tom-report") {

    setupUi(this);
#ifdef TOM_REPORTS
    _webView = new QWebEngineView(this);
    _webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
    _webView->settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    previewFrame->layout()->addWidget(_webView);
    _webView->show();
#elif
    previewFrame->hide();
#endif

    if (_tempDir.isValid()) {
        _tempFile = _tempDir.filePath("report.html");
    }

    connect(splitMoveUp, &QPushButton::pressed, [this] { moveSplitSelection(-1); });
    connect(splitMoveDown, &QPushButton::pressed, [this] { moveSplitSelection(1); });
    _splitModel->setCheckedItems(QStringList() << "project");
    splitList->setModel(_splitModel);
    splitList->setSelectionMode(QAbstractItemView::SingleSelection);
    splitList->adjustSize();

    dateStart->setDate(QDateTime::currentDateTime().date());
    dateEnd->setDate(QDateTime::currentDateTime().date());

    connect(updateButton, &QPushButton::pressed, this, &ProjectReportDialog::updateReport);

    connect(dateFilterCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(dateStart, &QDateEdit::dateChanged, this, &ProjectReportDialog::updateReport);
    connect(dateEnd, &QDateEdit::dateChanged, this, &ProjectReportDialog::updateReport);

    connect(frameRoundingMode, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);
    connect(frameRoundingValue, QOverload<int>::of(&QSpinBox::valueChanged), this, &ProjectReportDialog::updateReport);

    connect(templateBox, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);

    connect(_splitModel, &ReportSplitModel::itemStateChanged, this, &ProjectReportDialog::updateReport);
    connect(_splitModel, &ReportSplitModel::dataChanged, this, &ProjectReportDialog::updateReport);
    connect(_splitModel, &ReportSplitModel::modelReset, this, &ProjectReportDialog::updateReport);

    connect(matrixTablesCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showEmptyCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(showSummaryCheckbox, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);

    updateReport();
}

void ProjectReportDialog::updateReport() {
    QStringList projects;
    for (const auto &p:_projects) {
        if (p.isValid()) {
            projects << p.getID();
        }
    }

    QStringList splits = _splitModel->checkedItems();

    int frameRoundingMin = frameRoundingValue->value();

    const QString &frameModeText = frameRoundingMode->currentText();
    TimeRoundingMode frameMode = NONE;
    if (frameModeText == "up") {
        frameMode = UP;
    } else if (frameModeText == "down") {
        frameMode = DOWN;
    } else if (frameModeText == "up or down") {
        frameMode = NEAREST;
    }

    QDate start;
    QDate end;
    if (dateFilterCheckbox->isChecked()) {
        start = dateStart->date();
        end = dateEnd->date();
    }

    QString html = _control->htmlReport(_tempFile, projects, start, end, frameMode, frameRoundingMin,
                                        splits, templateBox->currentText(),
                                        matrixTablesCheckbox->isChecked(),
                                        showEmptyCheckbox->isChecked(),
                                        showSummaryCheckbox->isChecked(),
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
            const QModelIndex &newRow = selected.siblingAtRow(row + delta + (delta < 1 ? 0 : 0));
            splitList->selectionModel()->setCurrentIndex(newRow, QItemSelectionModel::SelectCurrent);
        }
    }
}
