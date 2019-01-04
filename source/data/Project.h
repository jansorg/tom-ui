#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

class Project {
public:
    Project(QString name, QString id);

    QString &getName();

    QString &getID();

private:
    QString name;
    QString id;
};


#endif //GOTIME_UI_PROJECT_H
