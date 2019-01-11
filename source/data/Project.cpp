#include "Project.h"

Project::Project() = default;

Project::Project(const QString &name, const QString &id, const QString &parentID) : _name(name),
                                                                                    _id(id),
                                                                                    _parentID(parentID) {}

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
