#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

#include "timespan/timespan.h"

class Project {
public:
    Project();

    Project(const QString &name, const QString &id, const QString &parentID,
            const Timespan &trackedYear, const Timespan &trackedMonth, const Timespan &trackedWeek,
            const Timespan &trackedDay);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

    QString getShortName() const;

    bool isValid() const;

    Timespan trackedYear() const;

    Timespan trackedMonth() const;

    Timespan trackedWeek() const;

    Timespan trackedDay() const;

private:
    QString _id;
    QString _name;
    QString _parentID;
    Timespan _year;
    Timespan _month;
    Timespan _week;
    Timespan _day;
};


#endif //GOTIME_UI_PROJECT_H
