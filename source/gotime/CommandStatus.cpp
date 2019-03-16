#include <utility>

#include <utility>

#include "CommandStatus.h"

CommandStatus::CommandStatus(QString _stdout, QString _stderr, int exitCode) : stdoutContent(std::move(_stdout)),
                                                                               stderrContent(std::move(_stderr)),
                                                                               exitCode(exitCode) {

}

bool CommandStatus::isSuccessful() const {
    return exitCode == 0;
}

bool CommandStatus::isFailed() const {
    return exitCode != 0;
}
