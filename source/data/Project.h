#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

class Project {
public:
    Project(QString &name, QString &id, QString &parentID);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

private:
    QString id;
    QString name;
    QString parentID;
};


#endif //GOTIME_UI_PROJECT_H
