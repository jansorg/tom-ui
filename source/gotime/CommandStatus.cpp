//
// Created by jansorg on 04.01.19.
//

#include "CommandStatus.h"

CommandStatus::CommandStatus(QString &stdout, QString &stderr, int exitCode) : stdoutContent(stdout),
                                                                               stderrContent(stderr),
                                                                               exitCode(exitCode) {

}

const bool CommandStatus::isSuccessful() {
    return exitCode == 0;
}

const bool CommandStatus::isFailed() {
    return exitCode != 0;
}
