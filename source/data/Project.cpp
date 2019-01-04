#include "Project.h"

Project::Project(QString name, QString id) {
    this->name = name;
    this->id = id;
}

QString& Project::getName() {
    return this->name;
}

QString& Project::getID() {
    return this->id;
}
