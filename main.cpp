#include <QApplication>
#include "ResourceManager.h"
#include <QIcon>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/image/tietie.jpeg"));
    ResourceManager w;
    w.show();
    return a.exec();
}
