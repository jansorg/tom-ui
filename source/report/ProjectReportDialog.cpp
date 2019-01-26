#include <utility>

#include <QtGlobal>

#include "ProjectReportDialog.h"

ProjectReportDialog::ProjectReportDialog(QList<Project> projects, TomControl *control, QWidget *parent)
        : QDialog(parent), _projects(std::move(projects)), _control(control) {

    setupUi(this);

    connect(updateButton, &QPushButton::pressed, this, &ProjectReportDialog::updateReport);
    connect(splitByYear, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(splitByMonth, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(splitByDay, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);
    connect(splitByProject, &QCheckBox::stateChanged, this, &ProjectReportDialog::updateReport);

    connect(frameRoundingMode, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);
    connect(frameRoundingValue, QOverload<int>::of(&QSpinBox::valueChanged), this, &ProjectReportDialog::updateReport);

    connect(templateBox, QOverload<int>::of(&QComboBox::activated), this, &ProjectReportDialog::updateReport);

    dateEnd->setDate(QDateTime::currentDateTime().date());

    updateReport();
}

void ProjectReportDialog::updateReport() {
    QStringList projects;
    for (const auto &p:_projects) {
        if (p.isValid()) {
            projects << p.getID();
        }
    }

    QStringList splits;
    if (splitByYear->isChecked()) {
        splits << "year";
    }
    if (splitByMonth->isChecked()) {
        splits << "month";
    }
    if (splitByDay->isChecked()) {
        splits << "day";
    }
    if (splitByProject->isChecked()) {
        splits << "project";
    }

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
    reportView->setContent(html.toUtf8(), "text/html");
}
