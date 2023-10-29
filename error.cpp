#include "error.h"
#include "ui_error.h"

Error::Error(QWidget *parent, QString text) :
    QDialog(parent),
    ui(new Ui::Error)
{
    ui->setupUi(this);
    ui->errorMsg->setText(text);
}

Error::~Error()
{
    delete ui;
}

void Error::on_buttonBox_clicked(QAbstractButton *button) {
    this->close();
}

