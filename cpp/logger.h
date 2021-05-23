#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <iostream>

#define DEBUGMODE 1
#define INFOMODE 1
#define WARNMODE 1


namespace logs {
inline void debug(QString d) {

#ifdef DEBUGMODE
    std::cout << "D/" << QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toStdString()  << "  " << d.toStdString() << std::endl;
#else
    Q_UNUSED(d)
#endif
}

inline void info(QString d) {

#ifdef DEBUGMODE
    std::cout << "I/" << QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toStdString() << "  " << d.toStdString() << std::endl;
#else
    Q_UNUSED(d)
#endif
}

inline void warn(QString d) {

#ifdef DEBUGMODE
    std::cerr << "W/" << QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toStdString() << "  " << d.toStdString() << std::endl;;
#else
    Q_UNUSED(d)
#endif
}
}

#endif // LOGGER_H
