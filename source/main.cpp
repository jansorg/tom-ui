#include <QApplication>
#include <QtWidgets/QMessageBox>

#include "main_window.h"
#include "gotime_tray_icon.h"

int main(int argc, char *argv[]) {
    QString command = "tom";
    bool bash = false;

    #ifdef Q_OS_MAC
    command = "./tom";
    #endif

    if (argc >= 2) {
        command = QString(argv[1]);
    }

    if (argc == 3) {
        bash = QString(argv[2]) == "true";
    }

    QApplication::setQuitOnLastWindowClosed(false);
    QApplication app(argc, argv);

    GotimeControl *control = new GotimeControl(command, bash, nullptr);
    MainWindow mainWindow(control);
    mainWindow.show();

    new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
