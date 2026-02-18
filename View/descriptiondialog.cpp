#include "DescriptionDialog.h"
#include "ui_DescriptionDialog.h"

DescriptionDialog::DescriptionDialog(const QString &text, QWidget *parent) :

    QDialog(parent), ui(new Ui::DescriptionDialog)
{
    ui->setupUi(this);
    ui->textEdit->setText(text);
}

DescriptionDialog::~DescriptionDialog()
{
    delete ui;
}

QString DescriptionDialog::getDescription() const {
    return ui->textEdit->toPlainText();
}
