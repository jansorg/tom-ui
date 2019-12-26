#ifndef TOM_UI_PROJECTSTATUSMANAGER_H
#define TOM_UI_PROJECTSTATUSMANAGER_H

#include <QtCore/QObject>
#include "TomControl.h"

class ProjectStatusManager : public QObject {
Q_OBJECT
public:
    ProjectStatusManager(TomControl *control, QObject *parent);

    ProjectStatus getStatus(const QString &projectID) const;

    ProjectStatus getOverallStatus() const;

public slots:

    void setIncludeArchived(bool includeArchived);

signals:

    void projectsStatusChanged(const QStringList &projectIDs);

private slots:

    void refresh();

private:
    /**
     * Loads a new status map from the tom cli application.
     */
    ProjectsStatus loadStatus();

    QTimer *_timer;
    TomControl *_control;

    ProjectsStatus _statusCache;
    bool _includeArchived = true;
};


#endif //TOM_UI_PROJECTSTATUSMANAGER_H
