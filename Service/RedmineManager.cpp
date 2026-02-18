#include "RedmineManager.h"
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrlQuery>

RedmineManager::RedmineManager(QObject *parent) : QObject(parent)
{
    network = new QNetworkAccessManager(this);
}

void RedmineManager::setAuth(QString endpointUrl, QString key)
{
    url = endpointUrl;
    apiKey = key;
}

void RedmineManager::fetch()
{
    if(url.isEmpty()) return;
    QUrl qurl(url + "/issues.json");
    QUrlQuery query;
    query.addQueryItem("limit", "100");
    query.addQueryItem("sort", "updated_on:desc");
    query.addQueryItem("include", "custom_fields");
    qurl.setQuery(query);

    QNetworkRequest request(qurl);
    request.setRawHeader("X-Redmine-API-Key", apiKey.toUtf8());

    QNetworkReply *r = network->get(request);
    connect(r, &QNetworkReply::finished, this, [this, r](){
        if(r->error() == QNetworkReply::NoError) {
            QJsonObject root = QJsonDocument::fromJson(r->readAll()).object();
            emit tasksReceived(root["issues"].toArray());
        }
        r->deleteLater();
    });
}

void RedmineManager::updateIssue(int id, const QJsonObject& data)
{
    if(url.isEmpty()) return;
    QString path = QString("%1/issues/%2.json").arg(url).arg(id);

    QNetworkRequest request((QUrl(path)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Redmine-API-Key", apiKey.toUtf8());

    QJsonObject root;
    root["issue"] = data;
    QNetworkReply *r = network->put(request, QJsonDocument(root).toJson());

    connect(r, &QNetworkReply::finished, this, [this, r]()
    {
        emit uploadFinished(r->error() == QNetworkReply::NoError);
        r->deleteLater();
    });
}
