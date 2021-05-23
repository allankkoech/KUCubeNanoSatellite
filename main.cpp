#include <QCoreApplication>
#include <QDebug>

#include "cpp/satcontroller.h"
#include "cpp/logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    logs::debug("Starting KU Nano Satellite 2021");

    SatController sat;

    return a.exec();

    logs::debug("Ending KU Nano Satellite 2021");

    // std::cin.get();
}
