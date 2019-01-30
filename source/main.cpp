#include <QApplication>
#include <QtWidgets/QMessageBox>
#include <source/projectlookup/projectlookup.h>

#include "qxt/qxtglobalshortcut.h"
#include "gotime/ProjectStatusManager.h"
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

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setQuitOnLastWindowClosed(false);
#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    QApplication app(argc, argv);

#ifdef Q_OS_MAC
    // locate binary next to our binary in the app bundle
    command = QFileInfo(QCoreApplication::applicationFilePath()).dir().filePath("tom");
#endif

    auto *control = new TomControl(command, bash, nullptr);
    auto *statusManager = new ProjectStatusManager(control, nullptr);

    MainWindow mainWindow(control, statusManager);
    mainWindow.show();

    new GotimeTrayIcon(control, &mainWindow);

    const QKeySequence shortcut(Qt::ShiftModifier + Qt::ControlModifier + Qt::Key_P);
    const QxtGlobalShortcut globalShortcut(shortcut);
    if (globalShortcut.isValid()) {
        QObject::connect(
                &globalShortcut, &QxtGlobalShortcut::activated, &globalShortcut,
                [&] {
                    ProjectLookup::show(control, nullptr);
                });
    } else {
        qWarning() << "failed to install global shortcut";
    }

    return QApplication::exec();
}
