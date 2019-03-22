#include <utility>

#include "Frame.h"

Frame::Frame() : id(""), projectID(""), startTime(), stopTime(), lastUpdated(), notes(""), tags(), archived(false) {
}

Frame::Frame(QString id,
             QString projectID,
             QDateTime start,
             QDateTime end,
             QDateTime lastUpdated,
             QString notes,
             QStringList tags,
             bool archived) : id(std::move(id)),
                              projectID(std::move(projectID)),
                              startTime(std::move(start)), stopTime(std::move(end)),
                              lastUpdated(std::move(lastUpdated)),
                              notes(std::move(notes)),
                              tags(std::move(tags)),
                              archived(archived) {
}

const Timespan Frame::getDuration() const {
    return Timespan::of(startTime, stopTime);
}
