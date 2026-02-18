#ifndef TASK_H
#define TASK_H
#include <QString>
#include <QMap>

struct Task {
    int id = 0;
    QString project;
    QString tracker;
    QString subject;
    QString description;
    QString status;
    int statusId = 0;
    QString priority;
    int priorityId = 0;
    QString author;
    QString startDate;
    QString dueDate;
    int doneRatio = 0;
    QString updatedOn;
    QMap<QString, QString> customFields;
    bool isModifiedLocally = false;
};
#endif
