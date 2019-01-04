#ifndef GOTIME_UI_STARTPROJECTACTION_H
#define GOTIME_UI_STARTPROJECTACTION_H


#include <QtWidgets/QAction>
#include <data/Project.h>

class StartProjectAction : public QAction {
Q_OBJECT

public:
    explicit StartProjectAction(Project *project, QObject *parent);

signals:
    void projectTriggered(Project *project);

private slots:
    void slotForwardStartProject();

private:
    Project *m_project;
};


#endif //GOTIME_UI_STARTPROJECTACTION_H
