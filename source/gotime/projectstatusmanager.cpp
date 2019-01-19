#include "projectstatusmanager.h"

ProjectStatusManager::ProjectStatusManager(GotimeControl *control, QObject *parent) : QObject(parent), _control(control) {
    refresh();

    connect(_control, &GotimeControl::projectUpdated, this, &ProjectStatusManager::refresh);
    connect(_control, &GotimeControl::frameUpdated, this, &ProjectStatusManager::refresh);
    connect(_control, &GotimeControl::frameRemoved, this, &ProjectStatusManager::refresh);

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
    //fixme we could diff old vs new state and only update the modified project status info. atm it's quick enough
    _statusCache = loadStatus();
    emit projectsStatusChanged(_statusCache.getMapping().keys());

}

ProjectsStatus ProjectStatusManager::loadStatus() {
    return _control->projectsStatus();
}
