#include <utility>

#include <utility>

#include "Project.h"

Project::Project() : _id(QString()),
                     _parentID(QString()),
                     _names(QStringList()),
                     _hourlyRate(""),
                     _lastUpdated(QDateTime()),
                     _fullName(""),
                     _isValid(false),
                     _isRootProject(false) {}

Project::Project(QStringList names, QString id, QString parentID, QString hourlyRate) : _id(std::move(id)),
                                                                                        _parentID(std::move(parentID)),
                                                                                        _names(std::move(names)),
                                                                                        _hourlyRate(std::move(hourlyRate)),
                                                                                        _isRootProject(false) {
    _fullName = _names.join("/");
    _isValid = !_id.isEmpty() && !_names.isEmpty();
}

QString Project::getName() const {
    return _fullName;
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
    return _isValid;
}

bool Project::isRootProject() const {
    return _isRootProject;
}

bool Project::isValidOrRootProject() const {
    return _isValid || _isRootProject;
}

QString Project::getHourlyRate() const {
    return _hourlyRate;
}
