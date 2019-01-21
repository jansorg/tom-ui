#ifndef GOTIME_UI_FRAME_H
#define GOTIME_UI_FRAME_H

#include <QtCore/QDateTime>
#include <timespan/timespan.h>

class Frame {
public:
    Frame();

    Frame(QString id, QString projectID, QDateTime start, QDateTime end, QDateTime lastUpdated, QString notes, QStringList tags);

    const Timespan getDuration() const;

    QString id;
    QString projectID;
    QDateTime startTime;
    QDateTime stopTime;
    QDateTime lastUpdated;
    QString notes;
    QStringList tags;

    inline bool isStopped() {
        return stopTime.isValid();
    }

    inline bool isRunning() {
        return !stopTime.isValid();
    }

    inline bool isSpanningMultipleDays() {
        return isStopped() && llabs(startTime.daysTo(stopTime)) >= 1;
    }

    inline qint64 durationMillis(bool includeActive) {
        if (isRunning() && includeActive) {
            return QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
        } else if (isRunning()) {
            return 0;
        }
        return stopTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
    }
};


#endif //GOTIME_UI_FRAME_H
