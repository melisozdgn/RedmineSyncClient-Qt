#ifndef DESCRIPTIONDIALOG_H
#define DESCRIPTIONDIALOG_H
#include <QDialog>
namespace Ui { class DescriptionDialog; }
class DescriptionDialog : public QDialog {
    Q_OBJECT
public:
    explicit DescriptionDialog(const QString &text, QWidget *parent = nullptr);
    ~DescriptionDialog();
    QString getDescription() const;
private:
    Ui::DescriptionDialog *ui;
};
#endif
