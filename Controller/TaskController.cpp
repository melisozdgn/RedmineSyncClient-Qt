#include "TaskController.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>
#include <QDate>
#include <utility>

TaskController::TaskController(QObject *parent) : QObject(parent)
{
    model = new QStandardItemModel(this);
    QStringList headers;
    headers << "ID" << "Project" << "Tracker" << "Subject" << "Status"
            << "Priority" << "Description" << "Start Date" << "Due Date"
            << "Done %" << "Author" << "Updated On";
    model->setHorizontalHeaderLabels(headers);
    redmine = new RedmineManager(this);
    connect(redmine, &RedmineManager::tasksReceived, this, &TaskController::onTasksReceived);
    connect(redmine, &RedmineManager::uploadFinished, this, &TaskController::onUploadFinished);
}

void TaskController::init()
{
    if (config.load()) {
        redmine->setAuth(config.url, config.apiKey);
    }
    tasks = repo.loadTasks();
    refresh();
}

void TaskController::sync()
{
    redmine->fetch();
}

void TaskController::onTasksReceived(QJsonArray data)
{
    if(isSyncingAfterPush) {
        isSyncingAfterPush = false;
    }

    for (const QJsonValue &val : std::as_const(data))
    {
        QJsonObject obj = val.toObject();
        Task serverTask;
        serverTask.id = obj["id"].toInt();
        serverTask.subject = obj["subject"].toString();
        serverTask.description = obj["description"].toString();
        serverTask.updatedOn = obj["updated_on"].toString();
        serverTask.startDate = obj["start_date"].toString();
        serverTask.dueDate = obj["due_date"].toString();
        serverTask.doneRatio = obj["done_ratio"].toInt();

        if(obj.contains("project")) serverTask.project = obj["project"].toObject()["name"].toString();

        if(obj.contains("status"))
        {
            QJsonObject sObj = obj["status"].toObject();
            serverTask.status = sObj["name"].toString();
            serverTask.statusId = sObj["id"].toInt();
        }

        if(obj.contains("tracker")) serverTask.tracker = obj["tracker"].toObject()["name"].toString();

        if(obj.contains("priority"))
        {
            QJsonObject pObj = obj["priority"].toObject();
            serverTask.priority = pObj["name"].toString();
            serverTask.priorityId = pObj["id"].toInt();
        }

        if(obj.contains("author")) serverTask.author = obj["author"].toObject()["name"].toString();

        if (obj.contains("custom_fields"))
        {
            QJsonArray customFields = obj["custom_fields"].toArray();
            for (const QJsonValue &c : std::as_const(customFields)) {
                QJsonObject cfObj = c.toObject();
                serverTask.customFields.insert(cfObj["name"].toString(), cfObj["value"].toString());
            }
        }

        bool foundLocally = false;
        for (int i = 0; i < tasks.size(); ++i)
        {
            if (tasks[i].id == serverTask.id)
            {
                foundLocally = true;
                if (tasks[i].isModifiedLocally)
                {
                    if (serverTask.updatedOn != tasks[i].updatedOn)
                    {
                        emit conflictFound(i, serverTask);
                    }
                } else {
                    tasks[i] = serverTask;
                    tasks[i].isModifiedLocally = false;
                }
                break;
            }
        }
        if (!foundLocally)
        {
            serverTask.isModifiedLocally = false;
            tasks.append(serverTask);
        }
    }
    save();
    pushLocalChanges();
}

void TaskController::pushLocalChanges()
{
    for (const Task& t : std::as_const(tasks))
    {
        if (t.isModifiedLocally)
        {
            QJsonObject issue;
            issue["subject"] = t.subject;
            issue["description"] = t.description;
            if(t.statusId > 0) issue["status_id"] = t.statusId;
            if(t.priorityId > 0) issue["priority_id"] = t.priorityId;
            issue["done_ratio"] = t.doneRatio;
            if(!t.startDate.isEmpty()) issue["start_date"] = t.startDate;
            if(!t.dueDate.isEmpty()) issue["due_date"] = t.dueDate;
            issue["notes"] = "Updated from Local App";
            redmine->updateIssue(t.id, issue);
        }
    }
}

void TaskController::onUploadFinished(bool success)
{
    if(success)
    {
        for(Task& t : tasks) t.isModifiedLocally = false;
        save();
        if(!isSyncingAfterPush) {
            isSyncingAfterPush = true;
            redmine->fetch();
        }
    }
}

void TaskController::update(int row, const Task& t)
{
    if(row >= 0 && row < tasks.size()) {
        tasks[row] = t;
        tasks[row].isModifiedLocally = true;
        save();
    }
}

void TaskController::resolveConflict(int row, const Task& serverTask)
{
    if (row >= 0 && row < tasks.size())
    {
        tasks[row] = serverTask;
        tasks[row].isModifiedLocally = false;
        save();
    }
}

void TaskController::keepLocalAndPush(int row)
{
    if(row >= 0 && row < tasks.size())
    {
        tasks[row].isModifiedLocally = true;

        QJsonObject issue;
        issue["subject"] = tasks[row].subject;
        issue["description"] = tasks[row].description;
        if(tasks[row].statusId > 0) issue["status_id"] = tasks[row].statusId;
        if(tasks[row].priorityId > 0) issue["priority_id"] = tasks[row].priorityId;
        issue["done_ratio"] = tasks[row].doneRatio;
        if(!tasks[row].startDate.isEmpty()) issue["start_date"] = tasks[row].startDate;
        if(!tasks[row].dueDate.isEmpty()) issue["due_date"] = tasks[row].dueDate;
        issue["notes"] = "Updated from Local App (Conflict Resolved: Kept Local)";

        redmine->updateIssue(tasks[row].id, issue);

        save();
    }
}

void TaskController::save()
{
    repo.saveTasks(tasks);
    refresh();
}

void TaskController::refresh()
{
    model->removeRows(0, model->rowCount());

    for (const Task& t : std::as_const(tasks))
    {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(t.id));
        row << new QStandardItem(t.project);
        row << new QStandardItem(t.tracker);
        row << new QStandardItem(t.subject);
        row << new QStandardItem(t.status);
        row << new QStandardItem(t.priority);
        row << new QStandardItem(t.description);
        row << new QStandardItem(t.startDate);
        row << new QStandardItem(t.dueDate);
        row << new QStandardItem(QString::number(t.doneRatio));
        row << new QStandardItem(t.author);
        row << new QStandardItem(t.updatedOn);

        QColor rowColor(Qt::white);
        if(t.isModifiedLocally) {
            rowColor = QColor(224, 242, 241);
        } else if(t.status == "Closed" || t.status == "Rejected" || t.doneRatio == 100) {
            rowColor = QColor(158, 158, 158);
        }

       for(int i = 0; i < row.size(); ++i)
        {
            row.at(i)->setBackground(QBrush(rowColor));
        }

        model->appendRow(row);
    }
    emit dataUpdated();
}

Task TaskController::getTask(int row)
{
    return (row >= 0 && row < tasks.size()) ? tasks[row] : Task();
}
