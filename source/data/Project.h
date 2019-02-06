#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

#include "timespan/timespan.h"

class Project {
public:
    Project();

    Project(QStringList names, QString id, QString parentID);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

    QString getShortName() const;

    bool isValid() const;

private:
    QString _id;
    QString _parentID;
    QStringList _names;
};

inline bool operator==(const Project &a, const Project &b) { return a.getID() == b.getID(); }

inline bool operator!=(const Project &a, const Project &b) { return !operator==(a, b); }

#endif //GOTIME_UI_PROJECT_H
