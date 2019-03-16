//
// Created by jansorg on 04.01.19.
//

#include <QtCore>

#ifndef GOTIME_UI_COMMANDSTATUS_H
#define GOTIME_UI_COMMANDSTATUS_H

class CommandStatus {
public:
    explicit CommandStatus(QString _stdout, QString _stderr, int exitCode);

    const QString stdoutContent;
    const QString stderrContent;
    const int exitCode;

    bool isSuccessful() const;

    bool isFailed() const;
};


#endif //GOTIME_UI_COMMANDSTATUS_H
