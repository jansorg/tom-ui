#include <QApplication>
#include <QtWidgets/QMessageBox>

#include "main_window.h"
#include "gotime_tray_icon.h"

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
    QApplication app(argc, argv);

    #ifdef Q_OS_MAC
        command = QFileInfo(QCoreApplication::applicationFilePath()).dir().filePath("tom");
    #endif

    GotimeControl *control = new GotimeControl(command, bash, nullptr);
    MainWindow mainWindow(control);
    mainWindow.show();

    new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
