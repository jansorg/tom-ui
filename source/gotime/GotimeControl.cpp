#include <QtCore/QProcess>
#include "GotimeControl.h"

GotimeControl::GotimeControl(QString gotimePath, QObject *parent) : _gotimePath(gotimePath), QObject(parent) {
}

QList<Project> GotimeControl::loadProjects() {
    CommandStatus status = run(QStringList() << "projects" << "-f" << "fullName,id,parentID");
    if (status.isFailed()) {
        return QList<Project>();
    }

    QStringList lines = status.stdoutContent.split("\n", QString::SkipEmptyParts);
    QList<Project> result;
    for (auto line : lines) {
        QStringList nameIdParent = line.split("\t");
        if (nameIdParent.size() == 3) {
            result.append(Project(nameIdParent[0], nameIdParent[1], nameIdParent[2]));
        }
    }
    return result;
}

bool GotimeControl::start(Project *project) {
    return run(QStringList() << "start" << project->getName()).isSuccessful();
}

bool GotimeControl::cancel() {
    return run(QStringList() << "cancel").isSuccessful();
}

bool GotimeControl::stop() {
    return run(QStringList() << "stop").isSuccessful();
}

CommandStatus GotimeControl::run(QStringList &args) {
    qDebug() << "running" << _gotimePath << args;

    QProcess process(this);
    process.start("/usr/bin/bash", QStringList() << _gotimePath << args);
    process.waitForFinished(500);

    QString output(process.readAllStandardOutput());
    QString errOutput(process.readAllStandardError());

    qDebug() << "exit code:" << process.exitCode();
    qDebug() << "exit status:" << process.exitStatus();
    qDebug() << "exit error:" << process.errorString();
    qDebug() << "stdout:" << output;
    qDebug() << "err stdout:" << errOutput;

    return CommandStatus(output, errOutput, process.exitCode());
}

GotimeStatus *GotimeControl::status() {
    return nullptr;
}