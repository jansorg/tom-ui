#ifndef GOTIME_UI_GOTIMECONTROL_H
#define GOTIME_UI_GOTIMECONTROL_H

#include <QtCore/QString>
#include <data/Project.h>

class GotimeControl : public QObject {
Q_OBJECT

public:
    GotimeControl(QString path, QObject *parent);

    void start(Project *project);
    void cancelWork();
    void stopWork();

private:
    void run(QStringList args);
    QString applicationPath;
};

#endif //GOTIME_UI_GOTIMECONTROL_H
