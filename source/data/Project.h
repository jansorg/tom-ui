#ifndef GOTIME_UI_PROJECT_H
#define GOTIME_UI_PROJECT_H

#include <QtCore>
#include <QtCore/QString>

#include "timespan/timespan.h"
#include "TriState.h"

class Project {
public:
    explicit Project();
//    Project(const Project& other);

    Project(QStringList names, QString id, QString parentID, QString hourlyRate, TriState noteRequired, bool noteRequiredInherited);

    QString getID() const;

    QString getParentID() const;

    QString getName() const;

    QString getShortName() const;

    QString getHourlyRate() const;

    TriState isNoteRequired() const;

    bool appliedIsNoteRequired() const;

    bool isValid() const;

    bool isRootProject() const;

    bool isValidOrRootProject() const;

    static Project rootProject() {
        Project root;
        root._isRootProject = true;
        return root;
    }

private:
    QString _id;
    QString _parentID;
    QStringList _names;
    QString _hourlyRate;
    TriState _noteRequired;
    bool _noteRequiredApplied;
    QDateTime _lastUpdated;
    QString _fullName;
    bool _isValid;
    bool _isRootProject;
};

inline bool operator==(const Project &a, const Project &b) { return a.getID() == b.getID(); }

inline bool operator!=(const Project &a, const Project &b) { return !operator==(a, b); }

#endif //GOTIME_UI_PROJECT_H
