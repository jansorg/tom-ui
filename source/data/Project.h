#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

#include "timespan/timespan.h"

class Project {
public:
    Project();

    Project(const QString &name, const QString &id, const QString &parentID);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

    QString getShortName() const;

    bool isValid() const;

private:
    QString _id;
    QString _name;
    QString _parentID;
};


#endif //GOTIME_UI_PROJECT_H
