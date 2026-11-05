//
// Created by jansorg on 04.01.19.
//

#include <QtCore>

#ifndef GOTIME_UI_COMMANDSTATUS_H
#define GOTIME_UI_COMMANDSTATUS_H

class CommandStatus {
public:
    explicit CommandStatus(QString _stdout, QString _stderr, int exitCode, QString errorMessage = QString());

    const QString stdoutContent;
    const QString stderrContent;
    const int exitCode;
    // set if the process failed to start, crashed or timed out
    const QString errorMessage;

    bool isSuccessful() const;

    bool isFailed() const;

    /**
     * @return a human-readable reason of the failure, empty if the command succeeded
     */
    QString failureReason() const;
};


#endif //GOTIME_UI_COMMANDSTATUS_H
