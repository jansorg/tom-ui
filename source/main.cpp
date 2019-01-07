#include <QApplication>
#include <QtWidgets/QMessageBox>
#include "main_window.h"
#include "gotime_tray_icon.h"

int main(int argc, char *argv[]) {
    QString command = "gotime";
    bool bash = false;

    if (argc >= 2) {
        command = QString(argv[1]);
    }
    if (argc == 3) {
        bash = QString(argv[2]) == "true";
    }

    GotimeControl *control = new GotimeControl(command, bash, nullptr);

    //  QApplication::setStyle("Fusion");
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);

    // we're hiding the main window by default, we only display the trayicon
    MainWindow mainWindow(control);
    mainWindow.show();

    auto *tray = new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
