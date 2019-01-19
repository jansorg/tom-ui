#include "projectstatusmanager.h"

ProjectStatusManager::ProjectStatusManager(GotimeControl *control, QObject *parent) : QObject(parent), _control(control) {
    refresh();

    connect(_control, &GotimeControl::projectUpdated, this, &ProjectStatusManager::refresh);
    connect(_control, &GotimeControl::frameUpdated, this, &ProjectStatusManager::refresh);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &ProjectStatusManager::refresh);

    // refresh every 30s
    _timer->start(30000);
}

ProjectStatus ProjectStatusManager::getStatus(const QString &projectID) const {
    return _statusCache.get(projectID);
}

// load new status, compare to old and emit signal for modified entries
void ProjectStatusManager::refresh() {
    const ProjectsStatus &newStatus = loadStatus();

    const auto oldMapping = _statusCache.getMapping();
    const QHash<QString, ProjectStatus> &newMapping = newStatus.getMapping();

    QStringList changedIDs;
    for (const auto &status : newMapping) {
        if (!oldMapping.contains(status.id) || oldMapping.value(status.id) != status) {
            changedIDs << status.id;
        }
    }

    _statusCache = newStatus;
    if (!changedIDs.isEmpty()) {
        emit projectsStatusChanged(changedIDs);
    }
}

ProjectsStatus ProjectStatusManager::loadStatus() {
    return _control->projectsStatus();
}
