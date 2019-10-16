#include"mainwindow.h"
#include"evolutionviewer.h"
#include"trading.h"
#include<QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
