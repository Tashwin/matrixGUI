/*
 * Author: Tashwin Khurana
 *
 * Runs the Matrix Window created in QtCreator
 */

#include "matrixwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MatrixWindow w;
    w.show();

    return a.exec();
}
