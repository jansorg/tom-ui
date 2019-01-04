#include "Project.h"

Project::Project(QString& name, QString &id, QString &parentID) : name(name), id(id), parentID(parentID){
}

QString Project::getName() const {
    return this->name;
}

QString Project::getID() const {
    return this->id;
}

QString Project::getParentID() const {
    return this->parentID;
}
