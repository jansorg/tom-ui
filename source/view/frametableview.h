//
// Created by jansorg on 19.01.19.
//

#ifndef TOM_UI_FRAMETABLEVIEW_H
#define TOM_UI_FRAMETABLEVIEW_H

#include <QtCore/QArgument>
#include <QtWidgets/QTableView>
#include <gotime/GotimeControl.h>

#include "data/Project.h"

class FrameTableView : public QTableView {
Q_OBJECT
public:
    explicit FrameTableView(QWidget *parent);

    void setup(GotimeControl *control);

public slots:
    void loadFrames(const Project &project);

private slots:
    void projectUpdated(const Project &project);

private:
    GotimeControl *_control;

    Project _currentProject;
};


#endif //TOM_UI_FRAMETABLEVIEW_H
