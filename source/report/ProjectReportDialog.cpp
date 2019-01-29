#include <utility>

#include <QtGlobal>

#include "ProjectReportDialog.h"
#include "reportsplitmodel.h"

ProjectReportDialog::ProjectReportDialog(QList<Project> projects, TomControl *control, QWidget *parent)
        : QDialog(parent), _projects(std::move(projects)), _control(control), _splitModel(new ReportSplitModel(this)) {

    setupUi(this);
#ifdef TOM_REPORT
    _webview = new QWebEngineView(this);
#endif

    connect(splitMoveUp, &QPushButton::pressed, [this] { moveSplitSelection(-1); });
    connect(splitMoveDown, &QPushButton::pressed, [this] { moveSplitSelection(1); });
    _splitModel->setCheckedItems(QStringList() << "project");
    splitList->setModel(_splitModel);
    splitList->adjustSize();

    dateEnd->setDate(QDateTime::currentDateTime().date());

    connect(updateButton, &QPushButton::pressed, this, &ProjectReportDialog::updateReport);

    connect(frameRoundingMode, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);
    connect(frameRoundingValue, QOverload<int>::of(&QSpinBox::valueChanged), this, &ProjectReportDialog::updateReport);

    connect(templateBox, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);

    connect(_splitModel, &ReportSplitModel::itemStateChanged, this, &ProjectReportDialog::updateReport);

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
    const QString &html = _control->htmlReport(projects,
                                               dateStart->date(), dateEnd->date(),
                                               frameMode, frameRoundingMin,
                                               splits, templateBox->currentText());

#ifdef TOM_REPORTS
    _webView->setHtml(html);
    _webView->show();
#endif
}

void ProjectReportDialog::moveSplitSelection(int delta) {
//    const QModelIndex &selected = splitList->selectionModel()->currentIndex();
//    if (selected.isValid()) {
//        int row = selected.row();
//        _splitModel->moveRow(row, delta, );
//    }
}
