#include "mainwindow.h"

#include <importer/eye-tracker-output/include/tobii_importer.h>
#include <importer/aoi/include/aoi-importer.h>
#include <slitscan-generator/include/genslitscan.h>

#include <opencv2/opencv.hpp>

#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QTextStream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    MainWindow w;
    w.show();


    return a.exec();
}
