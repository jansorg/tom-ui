#include "command_status.h"

command_status::command_status(QString &stdout, QString &stderr, int exitCode) : stdoutContent(stdout),
                                                                               stderrContent(stderr),
                                                                               exitCode(exitCode) {

}

const bool command_status::isSuccessful() {
    return exitCode == 0;
}

const bool command_status::isFailed() const {
    return exitCode != 0;
}
