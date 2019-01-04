#include <QtCore/QProcess>
#include "GotimeControl.h"

GotimeControl::GotimeControl(QString path, QObject *parent) : QObject(parent) {
    this->applicationPath = path;
}

void GotimeControl::start(Project *project) {
    run(QStringList() << "start" << project->getName());
}

void GotimeControl::cancelWork() {
    run(QStringList() << "cancel");
}

void GotimeControl::stopWork() {
    run(QStringList() << "stop");
}

void GotimeControl::run(QStringList args) {
    auto *process = new QProcess(this);

    process->start("/usr/bin/bash", QStringList() << applicationPath << args);
    process->waitForFinished(500);

    QString output(process->readAllStandardOutput());
    QString errOutput(process->readAllStandardError());

    qDebug() << "exit code:" << process->exitCode();
    qDebug() << "exit status:" << process->exitStatus();
    qDebug() << "exit error:" << process->errorString();
    qDebug() << "stdout:" << output;
    qDebug() << "err stdout:" << errOutput;
}
