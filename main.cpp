#include <QApplication>
#include "ResourceManager.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ResourceManager w;
    w.show();
    return a.exec();
}
