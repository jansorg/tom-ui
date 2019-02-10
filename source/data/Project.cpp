#include <utility>

#include "Project.h"

Project::Project() : _id(QString()),
                     _parentID(QString()),
                     _names(QStringList()),
                     _hourlyRate("") {

}

Project::Project(QStringList names, QString id, QString parentID, QString hourlyRate) : _id(std::move(id)),
                                                                                        _parentID(std::move(parentID)),
                                                                                        _names(std::move(names)),
                                                                                        _hourlyRate(std::move(hourlyRate)) {}

QString Project::getName() const {
    return this->_names.join("/");
}

QString Project::getShortName() const {
    if (_names.isEmpty()) {
        return "";
    }
    return _names.last();
}

QString Project::getID() const {
    return this->_id;
}

QString Project::getParentID() const {
    return this->_parentID;
}

bool Project::isValid() const {
    return !this->_id.isEmpty() && !_names.isEmpty();
}

QString Project::getHourlyRate() const {
    return _hourlyRate;
}
