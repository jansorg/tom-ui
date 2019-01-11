#include "Project.h"

//Project::Project() : _name(""), _id(""), _parentID(""),
//                     _year(Timespan()), _month(Timespan()), _week(Timespan()), _day(Timespan()) {}
Project::Project() = default;

Project::Project(const QString &name, const QString &id, const QString &parentID,
                 const Timespan &trackedYear,
                 const Timespan &trackedMonth,
                 const Timespan &trackedWeek,
                 const Timespan &trackedDay) : _name(name),
                                               _id(id),
                                               _parentID(parentID),
                                               _year(trackedYear),
                                               _month(trackedMonth),
                                               _week(trackedWeek),
                                               _day(trackedDay) {}

QString Project::getName() const {
    return this->_name;
}

QString Project::getShortName() const {
    QStringList parts = this->_name.split("/");
    return parts.last();
}

QString Project::getID() const {
    return this->_id;
}

QString Project::getParentID() const {
    return this->_parentID;
}

bool Project::isValid() const {
    return !this->_id.isEmpty();
}

Timespan Project::trackedYear() const {
    return _year;
}

Timespan Project::trackedMonth() const {
    return _month;
}

Timespan Project::trackedWeek() const {
    return _week;
}

Timespan Project::trackedDay() const {
    return _day;
}
