#ifndef REDMINEMANAGER_H
#define REDMINEMANAGER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>

class RedmineManager : public QObject {
    Q_OBJECT
public:
    explicit RedmineManager(QObject *parent = nullptr);
    void setAuth(QString url, QString apiKey);
    void fetch();
    void updateIssue(int id, const QJsonObject& data);

signals:
    void tasksReceived(QJsonArray tasks);
    void uploadFinished(bool success);

private:
    QNetworkAccessManager *network;
    QString url;
    QString apiKey;
};
#endif
