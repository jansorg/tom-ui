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
            Timespan year, Timespan yearTotal,
            Timespan month, Timespan monthTotal,
            Timespan week, Timespan weekTotal,
            Timespan day, Timespan dayTotal);

    QString id;
    Timespan year;
    Timespan yearTotal;
    Timespan month;
    Timespan monthTotal;
    Timespan week;
    Timespan weekTotal;
    Timespan day;
    Timespan dayTotal;
};

class ProjectsStatus {
public:
    ProjectsStatus();

    explicit ProjectsStatus(const QHash<QString, ProjectStatus> &mapping);

    ProjectStatus get(const QString &projectID) const;

    int size() const;

private:
    QHash<QString, ProjectStatus> _map;
};


#endif //TOM_UI_PROJECTSTATUS_H
