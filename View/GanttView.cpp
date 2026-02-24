#include "GanttView.h"
#include <QDate>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <QGraphicsSceneHoverEvent>
#include <QColor>

class GanttTaskItem : public QGraphicsRectItem
{
public:
    GanttTaskItem(qreal x, qreal y, qreal w, qreal h, QColor color, QString tooltip)
        : QGraphicsRectItem(x, y, w, h), m_color(color)
    {
        setBrush(QBrush(m_color));
        setPen(Qt::NoPen);
        setToolTip(tooltip);
        setAcceptHoverEvents(true);
        setFlag(QGraphicsItem::ItemIsSelectable, false);
    }
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override {
        setBrush(QBrush(m_color.lighter(108)));
        QGraphicsRectItem::hoverEnterEvent(event);
    }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override {
        setBrush(QBrush(m_color));
        QGraphicsRectItem::hoverLeaveEvent(event);
    }
private:
    QColor m_color;
};

GanttView::GanttView(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setBackgroundBrush(QBrush(Qt::white));
    setFrameShape(QFrame::NoFrame);
}

void GanttView::updateTasks(const QList<Task>& tasks)
{
    scene->clear();
    drawChart(tasks);
}

void GanttView::drawChart(const QList<Task>& tasks)
{
    if(tasks.isEmpty()) return;
    QDate minDate = QDate::currentDate();
    QDate maxDate = QDate::currentDate().addDays(30);

    for(const Task& t : tasks)
    {
        QDate s = QDate::fromString(t.startDate, "yyyy-MM-dd");
        QDate e = QDate::fromString(t.dueDate, "yyyy-MM-dd");
        if(s.isValid() && s < minDate) minDate = s;
        if(e.isValid() && e > maxDate) maxDate = e;
    }

    minDate = minDate.addDays(-3);
    maxDate = maxDate.addDays(7);
    int dayWidth = 46;
    int rowHeight = 42;
    int headerHeight = 55;
    int totalDays = minDate.daysTo(maxDate);

    for(int i=0; i <= totalDays; ++i)
    {
        QDate d = minDate.addDays(i);
        int x = i * dayWidth;
        if(d.dayOfWeek() == 6 || d.dayOfWeek() == 7)
        {
            scene->addRect(x, 0, dayWidth, (tasks.size() * rowHeight) + headerHeight + 80,
                           QPen(Qt::NoPen), QBrush(QColor(249, 250, 251)));
        }
        scene->addLine(x, 0, x, (tasks.size() * rowHeight) + headerHeight + 80, QPen(QColor(238, 238, 238)));
        QGraphicsTextItem* dayNum = scene->addText(d.toString("dd"));
        dayNum->setPos(x + 10, 8);
        QFont f = dayNum->font();
        f.setPointSize(10);
        f.setBold(true);
        dayNum->setFont(f);
        dayNum->setDefaultTextColor(QColor(66, 66, 66));
        QGraphicsTextItem* monthName = scene->addText(d.toString("MMM"));
        monthName->setPos(x + 8, 26);
        monthName->setDefaultTextColor(QColor(158, 158, 158));
        if(d == QDate::currentDate())
        {
            scene->addLine(x, headerHeight, x, (tasks.size() * rowHeight) + headerHeight, QPen(QColor(66, 165, 245), 2));
        }
    }
    scene->addLine(0, headerHeight, totalDays * dayWidth, headerHeight, QPen(QColor(224, 224, 224), 1));
    int y = headerHeight + 10;

    for(const Task& t : tasks)
    {
        QDate s = QDate::fromString(t.startDate, "yyyy-MM-dd");
        QDate e = QDate::fromString(t.dueDate, "yyyy-MM-dd");

        bool isOverdue = (s.isValid() && e.isValid() && e < s);

        if(!s.isValid()) s = QDate::currentDate();
        if(!e.isValid()) e = s.addDays(1);

        int drawE = (e < s) ? s.daysTo(s) : s.daysTo(e);
        int startX = minDate.daysTo(s) * dayWidth;
        int duration = drawE + 1;
        int width = duration * dayWidth;

        QColor taskColor(179, 229, 252);

        if(t.status == "Closed" || t.status == "Rejected" || t.doneRatio == 100)
        {
            taskColor = QColor(97, 97, 97);
        }
        else if(isOverdue)
        {
            taskColor = QColor(216, 67, 21);
        }
        else if(t.isModifiedLocally)
        {
            taskColor = QColor(128, 203, 196);
        }
        else if(t.priority == "High" || t.priority == "Immediate")
        {
            taskColor = QColor(239, 154, 154);
        }

        GanttTaskItem* bar = new GanttTaskItem(startX, y, width, rowHeight - 14, taskColor,
                                               QString("%1\n%2 -> %3").arg(t.subject).arg(t.startDate).arg(t.dueDate));
        scene->addItem(bar);

        if(t.doneRatio > 0)
        {
            int pWidth = (width * t.doneRatio) / 100;
            QGraphicsRectItem* pBar = scene->addRect(startX, y + (rowHeight - 17), pWidth, 3,
                                                     QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 120)));
            pBar->setZValue(2);
        }

        QString label = QString("#%1 %2").arg(t.id).arg(t.subject);
        QGraphicsTextItem* txt = scene->addText(label);
        txt->setPos(startX + 4, y - 2);
        if(t.status == "Closed" || t.status == "Rejected" || t.doneRatio == 100)
            txt->setDefaultTextColor(QColor(224, 224, 224));
        else
            txt->setDefaultTextColor(QColor(55, 71, 79));
        txt->setZValue(3);

        if(txt->boundingRect().width() > width)
        {
            txt->setOpacity(0);
            QGraphicsTextItem* outTxt = scene->addText(label);
            outTxt->setPos(startX + width + 8, y - 2);
            outTxt->setDefaultTextColor(QColor(96, 125, 139));
        }
        y += rowHeight;
    }

    int legendX = 20;
    int legendY = y + 20;
    auto addLegendItem = [&](QString text, QColor color)
    {
        scene->addRect(legendX, legendY, 15, 15, Qt::NoPen, QBrush(color));
        QGraphicsTextItem* t = scene->addText(text);
        t->setPos(legendX + 20, legendY - 3);
        t->setDefaultTextColor(Qt::black);
        QFont f = t->font();
        f.setPointSize(9);
        t->setFont(f);
        legendX += 140;
    };

    addLegendItem("Normal", QColor(179, 229, 252));
    addLegendItem("Completed", QColor(97, 97, 97));
    addLegendItem("Overdue", QColor(216, 67, 21));
    addLegendItem("Modified", QColor(128, 203, 196));
    addLegendItem("High Priority", QColor(239, 154, 154));

    scene->setSceneRect(0, 0, totalDays * dayWidth, legendY + 50);
}
