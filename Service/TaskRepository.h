#ifndef TASKREPOSITORY_H
#define TASKREPOSITORY_H
#include <QList>
#include "Model/Task.h"

class TaskRepository
{
public:

    void saveTasks(const QList<Task>& tasks);
    QList<Task> loadTasks();
};
#endif
