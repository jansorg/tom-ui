#include <utility>

#include "CommandStatus.h"

CommandStatus::CommandStatus(QString _stdout, QString _stderr, int exitCode, QString errorMessage)
        : stdoutContent(std::move(_stdout)),
          stderrContent(std::move(_stderr)),
          exitCode(exitCode),
          errorMessage(std::move(errorMessage)) {
}

bool CommandStatus::isSuccessful() const {
    return !isFailed();
}

bool CommandStatus::isFailed() const {
    return exitCode != 0 || !errorMessage.isEmpty();
}

QString CommandStatus::failureReason() const {
    if (!isFailed()) {
        return {};
    }
    if (!errorMessage.isEmpty()) {
        return errorMessage;
    }

    const QString stderrText = stderrContent.trimmed();
    if (!stderrText.isEmpty()) {
        return stderrText;
    }
    return QCoreApplication::translate("CommandStatus", "Exit code %1").arg(exitCode);
}
