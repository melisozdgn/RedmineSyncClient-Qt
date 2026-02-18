#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

void Logger::log(const QString& msg, Level lvl)
{
    QString lvlStr = (lvl == Info) ? "[INFO]" : (lvl == Warning) ? "[WARN]" : "[ERROR]";
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QString out = QString("[%1] %2 %3").arg(time, lvlStr, msg);
    qDebug().noquote() << out;

    QFile file("app.log");

    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << out << "\n";
    }
}
