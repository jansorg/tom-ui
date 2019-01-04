#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore/QString>
#include "data/Project.h"
#include "CommandStatus.h"
#include "GotimeStatus.h"


class GotimeControl : public QObject {
Q_OBJECT

public:
    explicit GotimeControl(QString gotimePath, QObject *parent);

//    void createProject(String name);
//    void createTag(String name);

//    void removeProject(Project project);

//    QList<Frame> loadFrames();
    QList<Project> loadProjects();
//    QList<Tag> loadTags();

    bool start(Project *project);
    bool stop();
    bool cancel();

    GotimeStatus *status();

private:
    CommandStatus run(QStringList &args);

    QString _gotimePath;
};

#endif //GOTIME_UI_GOTIMECONTROL_H
