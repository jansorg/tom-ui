#include "ProjectStatusManager.h"

ProjectStatusManager::ProjectStatusManager(TomControl *control, QObject *parent) : QObject(parent), _control(control) {
    refresh();

    connect(_control, &TomControl::dataResetNeeded, this, &ProjectStatusManager::refresh);

    connect(_control, &TomControl::projectStatusChanged, this, &ProjectStatusManager::refresh);
    connect(_control, &TomControl::projectUpdated, this, &ProjectStatusManager::refresh);
    connect(_control, &TomControl::projectFramesArchived, this, &ProjectStatusManager::refresh);

    connect(_control, &TomControl::framesUpdated, this, &ProjectStatusManager::refresh);
    connect(_control, &TomControl::framesRemoved, this, &ProjectStatusManager::refresh);
    connect(_control, &TomControl::framesMoved, this, &ProjectStatusManager::refresh);
    connect(_control, &TomControl::framesArchived, [this] {
        if (!_includeArchived) {
            refresh();
        }
    });

    // refresh project tree status every minute
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &ProjectStatusManager::refresh);
    _timer->start(60 * 1000);
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
    return _control->projectsStatus(ProjectStatus::OVERALL_ID, true, _includeArchived);
}

ProjectStatus ProjectStatusManager::getOverallStatus() const {
    return _statusCache.get(ProjectStatus::OVERALL_ID);
}

void ProjectStatusManager::setIncludeArchived(bool includeArchived) {
    if (includeArchived != _includeArchived) {
        _includeArchived = includeArchived;
        refresh();
    }
}
