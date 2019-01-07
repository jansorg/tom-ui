#include <QApplication>
#include <QtWidgets/QMessageBox>
#include "main_window.h"
#include "gotime_tray_icon.h"

int main(int argc, char *argv[]) {
    GotimeControl *control = new GotimeControl(QString("/home/jansorg/bin/gotime.build"), false, nullptr);

    //  QApplication::setStyle("Fusion");
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);

    // we're hiding the main window by default, we only display the trayicon
    MainWindow mainWindow(control);
    mainWindow.show();

    auto *tray = new GotimeTrayIcon(control, &mainWindow);
    return QApplication::exec();
}
