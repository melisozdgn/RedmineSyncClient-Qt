#ifndef CONFLICTDIALOG_H
#define CONFLICTDIALOG_H
#include <QDialog>
#include "../Model/Task.h"
namespace Ui { class ConflictDialog; }

class ConflictDialog : public QDialog
{
    Q_OBJECT

public:

    explicit ConflictDialog(const Task& local, const Task& server, QWidget *parent = nullptr);

    ~ConflictDialog();

    bool useLocal() const
    {
    return resultLocal;
    }

private slots:

    void onLocalClicked();
    void onServerClicked();

private:

    Ui::ConflictDialog *ui;
    bool resultLocal;
    QString getDiffHtml(const QString& mainText, const QString& otherText, const QString& color, const QString& bgColor);
    void buildComparison(const Task& local, const Task& server);

};
#endif
