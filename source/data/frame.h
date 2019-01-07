//
// Created by jansorg on 07.01.19.
//

#ifndef GOTIME_UI_FRAME_H
#define GOTIME_UI_FRAME_H


#include <QtCore/QDateTime>
#include <timespan/timespan.h>

class Frame {
public:
    Frame();
    Frame(QString id, QString projectID, QDateTime start, QDateTime end, QDateTime lastUpdated, QString notes, QStringList tags);

    const Timespan getDuration() const;

    const QString id;
    const QString projectID;
    const QDateTime startTime;
    const QDateTime stopTime;
    const QDateTime lastUpdated;
    const QString notes;
    const QStringList tags;
};


#endif //GOTIME_UI_FRAME_H
