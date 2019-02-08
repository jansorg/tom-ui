#include "Frame.h"

Frame::Frame() : id(""), projectID(""), startTime(), stopTime(), lastUpdated(), notes(""), tags(), archived(false) {
}

Frame::Frame(const QString &id,
             const QString &projectID,
             const QDateTime &start,
             const QDateTime &end,
             const QDateTime &lastUpdated,
             const QString &notes,
             const QStringList &tags,
             bool archived) : id(id), projectID(projectID),
                              startTime(start), stopTime(end), lastUpdated(lastUpdated),
                              notes(notes), tags(tags), archived(archived) {

}

const Timespan Frame::getDuration() const {
    return Timespan::of(startTime, stopTime);
}
