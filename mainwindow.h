#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "classes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QVector<Day> calendar;
    QMap<int, Employer> employers;
    QMap<QString, QVector<QPair<QDateTime, QDateTime>>> places;

signals:
    void signalEditButton(Event& event, MainWindow* _par);

private slots:
    void on_calendarWidget_clicked(const QDate &date);

    void on_AddButton_clicked();

    void on_OKbutton_clicked();

    void on_editEvent_clicked();

    void on_DeleteButton_clicked();

    void on_ChooseButton_clicked();

    void on_tabWidget2_tabBarClicked(int index);

    void on_addTime30M_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_addTime1H_clicked();

public slots:
    void slotEditFinished(Event& event);

private:
    Ui::MainWindow *ui;
    QDateTime cur_datetime;
    void throwError(QString msg);
    void addEventToTable(Event event, QTableWidget* eventTable);

};
#endif // MAINWINDOW_H
