#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <QObject>
#include <QStandardItemModel>
#include "../Model/Task.h"
#include "../Service/TaskRepository.h"
#include "../Service/RedmineManager.h"
#include "../Model/Config.h"

class TaskController : public QObject
{
    Q_OBJECT

public:
    explicit TaskController(QObject *parent = nullptr);
    void init();
    void sync();
    void update(int row, const Task& t);
    void resolveConflict(int row, const Task& serverTask);
    void keepLocalAndPush(int row);
    Task getTask(int row);

    QStandardItemModel* getModel() const
    {
        return model;
    }
    const QList<Task>& getTasks() const
    {
        return tasks;
    }

signals:
    void dataUpdated();
    void conflictFound(int row, Task serverTask);

private slots:
    void onTasksReceived(QJsonArray data);
    void onUploadFinished(bool success);

private:
    void save();
    void refresh();
    void pushLocalChanges();

    QStandardItemModel *model;
    TaskRepository repo;
    RedmineManager *redmine;
    Config config;
    QList<Task> tasks;
    bool isSyncingAfterPush = false;
};
#endif
