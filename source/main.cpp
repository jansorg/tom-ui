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

    gotime_control *control = new gotime_control(command, bash, nullptr);

    //  QApplication::setStyle("Fusion");
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);

    // we're hiding the main window by default, we only display the trayicon
    MainWindow mainWindow(control);
    mainWindow.show();

    new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
