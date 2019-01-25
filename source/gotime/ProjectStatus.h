#ifndef TOM_UI_PROJECTSTATUS_H
#define TOM_UI_PROJECTSTATUS_H

#include <QtCore/QHash>
#include <QtCore/QObject>

#include "timespan/timespan.h"

class ProjectStatus {

public:
    ProjectStatus();

    ProjectStatus(
            QString &id,
            Timespan all, Timespan allTotal,
            Timespan year, Timespan yearTotal,
            Timespan month, Timespan monthTotal,
            Timespan week, Timespan weekTotal,
            Timespan day, Timespan dayTotal);

    QString id;
    Timespan all;
    Timespan allTotal;
    Timespan year;
    Timespan yearTotal;
    Timespan month;
    Timespan monthTotal;
    Timespan week;
    Timespan weekTotal;
    Timespan day;
    Timespan dayTotal;

    static const QString OVERALL_ID;
};

class ProjectsStatus {
public:
    ProjectsStatus();

    explicit ProjectsStatus(const QHash<QString, ProjectStatus> &mapping);

    ProjectStatus get(const QString &projectID) const;

    int size() const;

    QStringList getProjectIDs();

    const QHash<QString, ProjectStatus> &getMapping() const;

    ProjectStatus getOverallStatus();

private:
    QHash<QString, ProjectStatus> _map;
};


inline bool operator==(const ProjectStatus &a, const ProjectStatus &b) {
    return a.id == b.id &&
           a.week == b.week &&
           a.weekTotal == b.weekTotal &&
           a.month == b.month &&
           a.monthTotal == b.monthTotal &&
           a.year == b.year &&
           a.yearTotal == b.yearTotal;
};

inline bool operator!=(const ProjectStatus &a, const ProjectStatus &b) { return !operator==(a, b); };


#endif //TOM_UI_PROJECTSTATUS_H
