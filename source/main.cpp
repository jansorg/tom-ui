#include <QApplication>
#include <QtWidgets/QMessageBox>
#include <gotime/projectstatusmanager.h>

#include "main_window.h"
#include "tray.h"

int main(int argc, char *argv[]) {
    QString command = "tom";
    bool bash = false;

    if (argc >= 2) {
        command = QString(argv[1]);
    }

    if (argc == 3) {
        bash = QString(argv[2]) == "true";
    }

    QApplication::setQuitOnLastWindowClosed(false);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    #ifdef Q_OS_MAC
        // locate binary next to our binary in the app bundle
        command = QFileInfo(QCoreApplication::applicationFilePath()).dir().filePath("tom");
    #endif

    auto *control = new GotimeControl(command, bash, nullptr);
    auto *statusManager = new ProjectStatusManager(control, nullptr);

    MainWindow mainWindow(control, statusManager);
    mainWindow.show();

    new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
