//
// Created by jansorg on 04.01.19.
//

#include <QtCore>

#ifndef GOTIME_UI_COMMANDSTATUS_H
#define GOTIME_UI_COMMANDSTATUS_H

class command_status {
public:
    explicit command_status(QString &stdout, QString &stderr, int exitCode);

    const QString stdoutContent;
    const QString stderrContent;
    const int exitCode;

    const bool isSuccessful();

    const bool isFailed() const;
};


#endif //GOTIME_UI_COMMANDSTATUS_H
