//
// Created by jansorg on 05.01.19.
//

#include "timespan.h"

Timespan::Timespan() : _msecs(0) {}

Timespan Timespan::of(const QDateTime &start, const QDateTime &end) {
    return Timespan(start.msecsTo(end));
}

Timespan::Timespan(const qint64 millis) : _msecs(millis) {
}

const QString Timespan::format() const {
    auto spanSecs = static_cast<qlonglong >(asSeconds());

    //3661 = 1h 1m 1s
    auto hours = spanSecs / 3600;
    auto minutes = spanSecs % 3600 / 60;
    auto seconds = spanSecs % 60;
    return QString("%1:%2:%3h")
            .arg(hours, 1, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
}

const QString Timespan::formatDecimal() const {
    return QString("%L1h").arg(asHours(), 0, 'g', 2);
}

const double Timespan::asHours() const {
    return asMinutes() / 60.0;
}

const double Timespan::asMinutes() const {
    return asSeconds() / 60.0;
}

const double Timespan::asSeconds() const {
    const auto &v = static_cast<double>(_msecs);
    return v / static_cast<double>(_msecsPerSecond);
}
