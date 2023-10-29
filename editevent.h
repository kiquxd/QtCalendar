#ifndef EDITEVENT_H
#define EDITEVENT_H

#include <QDialog>
#include "classes.h"
#include "mainwindow.h"

namespace Ui {
class EditEvent;
}

class EditEvent : public QDialog
{
    Q_OBJECT

public:
    explicit EditEvent(QWidget *parent = nullptr);
    ~EditEvent();

signals:
    void signalEditFinished(Event& event);

private:
    Ui::EditEvent *ui;
    MainWindow* par;
    void throwError(QString msg);

public slots:
    void slotEditButton(Event& event, MainWindow* _par);

private slots:
    void on_AddButton_clicked();

    void on_OKbutton_clicked();

    void on_DeleteButton_clicked();
};

#endif // EDITEVENT_H
