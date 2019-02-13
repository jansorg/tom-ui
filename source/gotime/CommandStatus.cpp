#include "CommandStatus.h"

CommandStatus::CommandStatus(const QString &stdout, const QString &stderr, int exitCode) : stdoutContent(stdout),
                                                                               stderrContent(stderr),
                                                                               exitCode(exitCode) {

}

bool CommandStatus::isSuccessful() const {
    return exitCode == 0;
}

bool CommandStatus::isFailed() const {
    return exitCode != 0;
}
