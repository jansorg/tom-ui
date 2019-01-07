//
// Created by jansorg on 07.01.19.
//

#include "frame.h"

Frame::Frame() : id(""), projectID(""), startTime(), stopTime(), lastUpdated(), notes(""), tags() {

}

Frame::Frame(const QString id,
             const QString projectID,
             const QDateTime start,
             const QDateTime end,
             const QDateTime lastUpdated,
             const QString notes,
             const QStringList tags) : id(id), projectID(projectID), startTime(start), stopTime(end),
                                        lastUpdated(lastUpdated), notes(notes), tags(tags) {

}

const Timespan Frame::getDuration() const {
    return Timespan::of(startTime, stopTime);
}
