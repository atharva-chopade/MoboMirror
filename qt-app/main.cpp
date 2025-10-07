#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("MoboMirror");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("MoboMirror");
    
    MainWindow w;
    w.show();
    return a.exec();
}
