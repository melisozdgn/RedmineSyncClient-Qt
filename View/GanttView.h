#ifndef GANTTVIEW_H
#define GANTTVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include "../Model/Task.h"

class GanttView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit GanttView(QWidget *parent = nullptr);
    void updateTasks(const QList<Task>& tasks);

private:

    QGraphicsScene *scene;
    void drawChart(const QList<Task>& tasks);
};
#endif
