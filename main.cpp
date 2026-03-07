#include <QApplication>
#include "ResourceManager.h"
#include <QIcon>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/image/tietie.jpeg"));
    QCoreApplication::setApplicationName("TIETIE");
    ResourceManager w;
    QStringList args = QApplication::arguments();
    if (!args.contains("--silent")) {
        // 只有非静默启动时（即用户手动双击打开时）才显示窗口
        w.show();
    }
    return a.exec();
}
