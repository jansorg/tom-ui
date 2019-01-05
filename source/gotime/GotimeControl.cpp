#include <utility>

#include <QtCore/QProcess>
#include "GotimeControl.h"

GotimeControl::GotimeControl(QString gotimePath, QObject *parent) : _gotimePath(std::move(gotimePath)),
                                                                    QObject(parent) {
}

QList<Project> GotimeControl::loadProjects() {
    CommandStatus status = run(QStringList() << "projects" << "-f" << "fullName,id,parentID");
    if (status.isFailed()) {
        return QList<Project>();
    }

    QStringList lines = status.stdoutContent.split("\n", QString::SkipEmptyParts);
    QList<Project> result;
    for (const auto &line : lines) {
        QStringList nameIdParent = line.split("\t");
        if (nameIdParent.size() == 3) {
            result.append(Project(nameIdParent[0], nameIdParent[1], nameIdParent[2]));
        }
    }
    return result;
}

bool GotimeControl::isStarted(Project &project) {
    const GotimeStatus &currentStatus = this->status();
    if (!currentStatus.isValid) {
        return false;
    }

    const Project &active = currentStatus.currentProject();
    qDebug() << "isStarted. active: " << active.getID();

    bool isActive = active.getID() == project.getID();
    if (isActive) {
        qDebug() << "found active project";
    } else {
        qDebug() << "not active" << project.getID();
    }
    return isActive;
}

bool GotimeControl::startProject(Project &project) {
    auto success = run(QStringList() << "start" << project.getName()).isSuccessful();
    if (success) {
        emit projectStarted(project);
    }
    return success;
}

bool GotimeControl::cancelActivity() {
    const GotimeStatus &active = status();

    bool success = run(QStringList() << "cancel").isSuccessful();
    if (success && active.isValid) {
        emit projectCancelled(active.currentProject());
        emit projectStopped(active.currentProject());
    }
    return success;
}

bool GotimeControl::stopActivity() {
    const GotimeStatus &current = status();

    bool success = run(QStringList() << "stop").isSuccessful();
    if (success && current.isValid) {
        emit projectStopped(current.currentProject());
    }
    return success;
}

CommandStatus GotimeControl::run(QStringList &args) {
    qDebug() << "running" << _gotimePath << args;

    QProcess process(this);
    process.start("/usr/bin/bash", QStringList() << _gotimePath << args);
    process.waitForFinished(500);

    QString output(process.readAllStandardOutput());
    QString errOutput(process.readAllStandardError());

//    qDebug() << "exit code:" << process.exitCode();
//    qDebug() << "exit status:" << process.exitStatus();
//    qDebug() << "exit error:" << process.errorString();
//    qDebug() << "stdout:" << output;
//    qDebug() << "err stdout:" << errOutput;

    return CommandStatus(output, errOutput, process.exitCode());
}

GotimeStatus GotimeControl::status() {
    // frameID, projectName, projectID, ...
    CommandStatus status = run(
            QStringList() << "status" << "-f" << "id,projectFullName,projectID,projectParentID,startTime");
    if (status.isFailed()) {
        return GotimeStatus();
    }

    // fixme atm we omly support a single active project
    QStringList lines = status.stdoutContent.split("\n");
    if (lines.isEmpty()) {
        return GotimeStatus();
    }

    QStringList parts = lines.first().split("\t");
    if (parts.size() != 5) {
        qDebug() << "unexpected status line" << lines.first();
        return GotimeStatus();
    }

    QDateTime startTime = QDateTime::fromString(parts[4], Qt::ISODate);
    Project project = Project(parts[1], parts[2], parts[3]);
    return GotimeStatus(true, project, startTime);
}
