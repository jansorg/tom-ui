#ifndef GOTIME_UI_FRAME_H
#define GOTIME_UI_FRAME_H

#include <QtCore/QDateTime>
#include <timespan/timespan.h>

class Frame {
public:
    Frame();

    Frame(const QString &id, const QString &projectID, const QDateTime &start, const QDateTime &end, const QDateTime &lastUpdated, const QString &notes, const QStringList &tags, bool archived);

    const Timespan getDuration() const;

    QString id;
    QString projectID;
    QDateTime startTime;
    QDateTime stopTime;
    QDateTime lastUpdated;
    QString notes;
    QStringList tags;
    bool archived;

    inline bool isStopped() {
        return stopTime.isValid();
    }

    inline bool isActive() {
        return !stopTime.isValid();
    }

    inline bool isSpanningMultipleDays() {
        return isStopped() && llabs(startTime.daysTo(stopTime)) >= 1;
    }

    inline qint64 durationMillis(bool includeActive) {
        if (isActive() && includeActive) {
            return QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
        } else if (isActive()) {
            return 0;
        }
        return stopTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
    }
};


#endif //GOTIME_UI_FRAME_H
