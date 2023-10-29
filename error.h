#ifndef ERROR_H
#define ERROR_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class Error;
}

class Error : public QDialog
{
    Q_OBJECT

public:
    explicit Error(QWidget *parent = nullptr, QString text = QString());
    ~Error();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::Error *ui;
};

#endif // ERROR_H
