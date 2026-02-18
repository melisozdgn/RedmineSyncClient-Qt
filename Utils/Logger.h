#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:

    enum Level { Info, Warning, Error };

    static void log(const QString& msg, Level lvl = Info);
};

#endif
