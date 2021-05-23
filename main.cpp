#include <QCoreApplication>
#include <QDebug>

#include "satcontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Starting KU Nano Satellite 2021";

    SatController sat;

    qDebug() << "Ending KU Nano Satellite 2021";

    return a.exec();
}
