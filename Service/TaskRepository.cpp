#include "TaskRepository.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <utility>

void TaskRepository::saveTasks(const QList<Task>& tasks)
{
    QJsonArray array;
    for (const Task& t : std::as_const(tasks))
    {
        QJsonObject obj;
        obj["id"] = t.id;
        obj["project"] = t.project;
        obj["tracker"] = t.tracker;
        obj["subject"] = t.subject;
        obj["description"] = t.description;
        obj["status"] = t.status;
        obj["status_id"] = t.statusId;
        obj["priority"] = t.priority;
        obj["priority_id"] = t.priorityId;
        obj["author"] = t.author;
        obj["start_date"] = t.startDate;
        obj["due_date"] = t.dueDate;
        obj["done_ratio"] = t.doneRatio;
        obj["updatedOn"] = t.updatedOn;
        obj["isModifiedLocally"] = t.isModifiedLocally;
        QJsonObject cfObj;
        for(auto it = t.customFields.begin(); it != t.customFields.end(); ++it) {
            cfObj[it.key()] = it.value();
        }
        obj["custom_fields_map"] = cfObj;
        array.append(obj);
    }

    QFile file("tasks.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(QJsonDocument(array).toJson());
    }
}

QList<Task> TaskRepository::loadTasks()
{
    QList<Task> list;
    QFile file("tasks.json");
    if (!file.open(QIODevice::ReadOnly)) return list;
    const QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();

    for (const QJsonValue &val : array) {
        QJsonObject obj = val.toObject();

        Task t;
        t.id = obj["id"].toInt();
        t.project = obj["project"].toString();
        t.tracker = obj["tracker"].toString();
        t.subject = obj["subject"].toString();
        t.description = obj["description"].toString();
        t.status = obj["status"].toString();
        t.statusId = obj["status_id"].toInt();
        t.priority = obj["priority"].toString();
        t.priorityId = obj["priority_id"].toInt();
        t.author = obj["author"].toString();
        t.startDate = obj["start_date"].toString();
        t.dueDate = obj["due_date"].toString();
        t.doneRatio = obj["done_ratio"].toInt();
        t.updatedOn = obj["updatedOn"].toString();
        t.isModifiedLocally = obj["isModifiedLocally"].toBool();
        QJsonObject cfObj = obj["custom_fields_map"].toObject();

        for(auto it = cfObj.begin(); it != cfObj.end(); ++it)
        {
            t.customFields.insert(it.key(), it.value().toString());
        }

        list.append(t);
    }
    return list;
}
