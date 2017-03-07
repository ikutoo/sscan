#include "cmainwin.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleFactory>
void setStyle(QApplication & a);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setStyle(a);
    CMainWin w;
    w.show();

    return a.exec();
}
void setStyle(QApplication & a) {
    a.setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(50, 50, 50));
    palette.setColor(QPalette::WindowText, Qt::white);
    a.setPalette(palette);

}
