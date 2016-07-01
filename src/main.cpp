#include <QtGui/QApplication>
#include <QDebug>
#include "qanaqitmaindlg.h"
#include "LoggerTime.h"

int main(int argc, char *argv[])
{
    LoggerTime::reset();
    QApplication a(argc, argv);
    QanaQitMainDlg w;
    w.show();
    //qDebug() << "Somewhere Here";
    return a.exec();
}
