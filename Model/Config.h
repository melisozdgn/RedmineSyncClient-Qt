#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

class Config {
public:
    QString url;
    QString apiKey;

    bool load() {
        QFile file("config.json");
        if (!file.open(QIODevice::ReadOnly)) return false;
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();

        QJsonObject src = obj["redmineSrc"].toObject();
        if(!src.isEmpty()){
            url = src["baseUrl"].toString();
            apiKey = src["restApiKey"].toString();
        } else {
            url = obj["url"].toString();
            apiKey = obj["apiKey"].toString();
        }
        return true;
    }
};
#endif
