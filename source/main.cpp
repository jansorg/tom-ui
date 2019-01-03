#include <QApplication>
#include <QtWidgets/QMessageBox>
#include "main_window.h"

int main(int argc, char* argv[])
{
  QApplication::setStyle("Fusion");
  QApplication::setQuitOnLastWindowClosed(false);

  QApplication app(argc, argv);

  // we're hiding the main window by default, we only display the trayicon
  MainWindow mainWindow;
  mainWindow.show();

  return QApplication::exec();
}
