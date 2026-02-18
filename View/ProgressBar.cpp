#include "ProgressBar.h"
#include <QApplication>
#include <QPainter>

ProgressBar::ProgressBar(QObject *parent) : QStyledItemDelegate(parent) {}

void ProgressBar::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    if (index.column() == 9)
    {
        int progress = index.data().toInt();
        QStyleOptionProgressBar progressBarOption;

        progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4);
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;
        progressBarOption.text = QString("%1%").arg(progress);
        progressBarOption.textVisible = true;
        progressBarOption.textAlignment = Qt::AlignCenter;

        painter->save();

        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
        painter->restore();

    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
