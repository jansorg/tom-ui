#include "Project.h"

Project::Project() = default;

Project::Project(const QStringList &names, const QString &id, const QString &parentID) : _id(id),
                                                                                         _parentID(parentID),
                                                                                         _names(names) {}

QString Project::getName() const {
    return this->_names.join("/");
}

QString Project::getShortName() const {
    return this->_names.last();
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
