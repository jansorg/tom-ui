#ifndef TOM_UI_REPORTDIALOG_H
#define TOM_UI_REPORTDIALOG_H

#include <QtWidgets/QDialog>
#include <QStringListModel>

#ifdef TOM_REPORTS
#include <QtWebEngineWidgets/QWebEngineView>
#endif

#include "ui_reportDialog.h"
#include "source/data/Project.h"
#include "source/gotime/TomControl.h"
#include "reportsplitmodel.h"

class ProjectReportDialog : public QDialog, private Ui::ReportDialog {
public:
    ProjectReportDialog(QList<Project> projects, TomControl *control, ProjectStatusManager *statusManager, QWidget *parent);

private slots:

    void updateReport();

    void projectIndexSelected(const QModelIndex& index);

private:
    void moveSplitSelection(int delta);

    QStringList _projects;
    TomControl *_control;
    ReportSplitModel *_splitModel;

    QTemporaryDir _tempDir;
    QString _tempFile;

    ProjectTreeModel *_projectModel;

#ifdef TOM_REPORTS
    QWebEngineView *_webView;
#endif
};

#endif //TOM_UI_REPORTDIALOG_H
