#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <QStyledItemDelegate>

class ProgressBar : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit ProgressBar(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};
#endif
