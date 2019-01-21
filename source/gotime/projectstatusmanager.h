#ifndef TOM_UI_PROJECTSTATUSMANAGER_H
#define TOM_UI_PROJECTSTATUSMANAGER_H

#include <QtCore/QObject>
#include "GotimeControl.h"

class ProjectStatusManager : public QObject {
Q_OBJECT
public:
    ProjectStatusManager(GotimeControl *control, QObject *parent);

    ProjectStatus getStatus(const QString &projectID) const;
    ProjectStatus getOverallStatus() const;

signals:

    void projectsStatusChanged(const QStringList &projectIDs);

private slots:

    void refresh();

private:
    /**
     * Loads a new status map from the tom cli application.
     * @return
     */
    ProjectsStatus loadStatus();

    QTimer *_timer;
    GotimeControl *_control;

    ProjectsStatus _statusCache;
};


#endif //TOM_UI_PROJECTSTATUSMANAGER_H
