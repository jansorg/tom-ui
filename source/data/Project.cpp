#include <utility>

#include "Project.h"

Project::Project() : _id(QString()),
                     _parentID(QString()),
                     _names(QStringList()),
                     _hourlyRate(QString()),
                     _noteRequired(UNDEFINED),
                     _noteRequiredApplied(false),
                     _lastUpdated(QDateTime()),
                     _fullName(QString()),
                     _isValid(false),
                     _isRootProject(false) {}

/*Project::Project(const Project& other)  : _id(other._id),
                                          _parentID(other._parentID),
                                          _names(other._names),
                                          _hourlyRate(other._hourlyRate),
                                          _noteRequired(other._noteRequired),
                                          _noteRequiredApplied(other._noteRequiredApplied),
                                          _lastUpdated(other._lastUpdated),
                                          _fullName(other._fullName),
                                          _isValid(other._isValid),
                                          _isRootProject(other._isRootProject) {}
*/

Project::Project(QStringList names, QString id, QString parentID, QString hourlyRate, TriState noteRequired, bool noteRequiredInherited)
        : _id(std::move(id)), _parentID(std::move(parentID)), _names(std::move(names)),
          _hourlyRate(std::move(hourlyRate)),
          _noteRequired(noteRequired), _noteRequiredApplied(noteRequiredInherited),
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

TriState Project::isNoteRequired() const {
    return _noteRequired;
}

bool Project::appliedIsNoteRequired() const {
    return _noteRequiredApplied;
}

