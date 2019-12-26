#ifndef GOTIME_UI_GOTIMESTATUS_H
#define GOTIME_UI_GOTIMESTATUS_H

#include <QtCore/QDateTime>
#include "data/Project.h"

class TomStatus {
public:
    explicit TomStatus();

    explicit TomStatus(bool valid, QString timeEntryId, Project &activeProject, QDateTime &startTime);

    const QString &timeEntryId() const;

    const Project &currentProject() const;

    const QDateTime &startTime() const;

    bool isValid;

private:
    QString _timeEntryId;
    Project _project;
    QDateTime _startTime;
};

inline bool operator==(const TomStatus &a, const TomStatus &b) { return a.currentProject() == b.currentProject() && a.isValid == b.isValid && a.startTime() == b.startTime(); }

inline bool operator!=(const TomStatus &a, const TomStatus &b) { return !operator==(a, b); }

#endif 
