#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

#include "timespan/timespan.h"

class Project {
public:
    Project();

    Project(const QStringList &names, const QString &id, const QString &parentID);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

    QString getShortName() const;

    bool isValid() const;

private:
    QString _id;
    QStringList _names;
    QString _parentID;
};


#endif //GOTIME_UI_PROJECT_H
