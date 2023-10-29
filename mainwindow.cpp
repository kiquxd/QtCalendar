#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "error.h"
#include "editevent.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    calendar.resize(31);
    cur_datetime.setDate(QDate(2024, 1, 1));
    cur_datetime.setTime(QTime(0, 0, 0));
    ui->dateTime->setText(cur_datetime.toString());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addEventToTable(Event event, QTableWidget* eventTable) {
    if (event.getStartDate() < cur_datetime) return;
    int size = eventTable->rowCount();
    eventTable->insertRow(size);
    eventTable->setItem(size, 0, new QTableWidgetItem(event.getName()));
    eventTable->setItem(size, 1, new QTableWidgetItem(event.getPlace()));
    eventTable->setItem(size, 2, new QTableWidgetItem(event.getStartDate().toString()));
    eventTable->setItem(size, 3, new QTableWidgetItem(event.getEndDate().toString()));
    QString members;
    for (auto& idx : event.getMembers()) {
        members += QString::number(idx);
        if (idx != event.getMembers().back()) {
            members += ", ";
        }
    }
    eventTable->setItem(size, 4, new QTableWidgetItem(members));
    eventTable->sortItems(2);
}

void MainWindow::throwError(QString msg) {
    Error* w = new Error(nullptr, msg);
    w->show();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date) {
    if (ui->tabWidget2->currentIndex() == 0) {
        ui->eventTable->setRowCount(0);
        for (auto& event : calendar[date.day() - 1].getEvents()) {
            addEventToTable(event, ui->eventTable);
        }
    } else {
        ui->oneManEventTable->setRowCount(0);
    }
}


void MainWindow::on_AddButton_clicked() {
    int st = ui->spinBox->value();
    QString text = QString::number(st);
    int ok = 1;
    for (int i = 0; i < ui->Participants->count(); ++i) {
        auto item = ui->Participants->item(i);
        if (item->text() == text) {
            ok = 0;
        }
    }
    if (ok) {
        ui->Participants->addItem(text);
    }
    ui->spinBox->setValue(0);
}


void MainWindow::on_DeleteButton_clicked() {
    if (ui->Participants->currentItem() == nullptr) return;
    delete ui->Participants->currentItem();
}


void MainWindow::on_OKbutton_clicked() {
    QVector<int> ids;
    QDateTime startDate = ui->startTime->dateTime();
    QDateTime endDate = ui->endTime->dateTime();
    QString place = ui->Place->text();
    QString name = ui->Name->text();
    if (place.size() == 0 || name.size() == 0) {
        throwError("Не заполнено место или время");
        return;
    }
    for (auto& [stTime, eTime] : places[place]) {
        if (stTime < startDate && eTime < startDate) continue;
        if (stTime > endDate && eTime > endDate) continue;
        throwError("В это время в " + place + " уже проходит другое\nсобытие");
        return;
    }
    for (int i = 0; i < ui->Participants->count(); ++i) {
        int id = ui->Participants->item(i)->text().toInt();
        ids.push_back(id);
    }
    Event event(name, startDate, endDate, place, ids);
    if (startDate > endDate || startDate.date().day() != endDate.date().day()) {
        throwError("Дата начала и дата окончания введены неко\nрректно");
        return;
    }
    for (int i = 0; i < ui->Participants->count(); ++i) {
        int id = ui->Participants->item(i)->text().toInt();
        if (employers.find(id) == employers.end()) {
            employers[id] = Employer(id);
        }
        for (auto& event : employers[id].getEvents()) {
            if (event.getStartDate() < startDate && event.getEndDate() < startDate) continue;
            if (event.getStartDate() > endDate && event.getEndDate() > endDate) continue;
            throwError("Мероприятие не подходит для участника с\nномером " + QString::number(ids.back()));
            return;
        }
    }
    if (ids.size() == 0) {
        throwError("Список участников пустой");
        return;
    }
    for (auto& id : ids) {
        if (employers.find(id) == employers.end()) {
            employers[id] = Employer(id);
        }
        employers[id].addEvent(event);
    }
    ui->Participants->clear();
    ui->Place->clear();
    ui->Name->clear();
    ui->startTime->setDateTime(ui->startTime->minimumDateTime());
    ui->endTime->setDateTime(ui->startTime->minimumDateTime());
    calendar[startDate.date().day() - 1].addEvent(event);
    addEventToTable(event, ui->allEvents);
    places[place].emplace_back(startDate, endDate);
}

void MainWindow::on_editEvent_clicked() {
    if (ui->allEvents->currentColumn() != 0) return;
    int row = ui->allEvents->currentRow();
    EditEvent* w = new EditEvent(this);
    connect(this, SIGNAL(signalEditButton(Event&, MainWindow*)), w, SLOT(slotEditButton(Event&, MainWindow*)));
    // name, place, startDate, endDate, participants
    QString name = ui->allEvents->item(row, 0)->text();
    QString place = ui->allEvents->item(row, 1)->text();
    QDateTime startDate = QDateTime::fromString(ui->allEvents->item(row, 2)->text());
    QDateTime endDate = QDateTime::fromString(ui->allEvents->item(row, 3)->text());
    QString str_ids = ui->allEvents->item(row, 4)->text();
    QVector<int> ids;
    for (int i = 0; i < str_ids.size(); i += 2) {
        int cur_id = 0;
        while (i < str_ids.size() && str_ids[i].isDigit()) {
            cur_id *= 10;
            cur_id += str_ids[i].unicode() - '0';
            ++i;
        }
        ids.push_back(cur_id);
    }
    Event event(name, startDate, endDate, place, ids);
    calendar[startDate.date().day() - 1].eraseEvent(event);
    for (auto& id : ids) {
        employers[id].eraseEvent(event);
    }
    places[place].erase(places[place].begin() + places[place].indexOf({startDate, endDate}));
    w->show();
    ui->allEvents->removeRow(row);
    emit signalEditButton(event, this);
}

void MainWindow::slotEditFinished(Event& event) {
    calendar[event.getStartDate().date().day() - 1].addEvent(event);
    addEventToTable(event, ui->allEvents);
    places[event.getPlace()].emplace_back(event.getStartDate(), event.getEndDate());
}


void MainWindow::on_ChooseButton_clicked() {
    int id = ui->spinBox2->value();
    ui->spinBox2->setValue(0);
    ui->oneManEventTable->setRowCount(0);
    int i = ui->calendarWidget->selectedDate().day() - 1;
    for (auto& event : calendar[i].getEvents()) {
        for (auto& idx : event.getMembers()) {
            if (idx == id) {
                addEventToTable(event, ui->oneManEventTable);
                break;
            }
        }
    }
}


void MainWindow::on_tabWidget2_tabBarClicked(int index) {
    if (index != 0) return;
    ui->eventTable->setRowCount(0);
    int i = ui->calendarWidget->selectedDate().day() - 1;
    for (auto& event : calendar[i].getEvents()) {
        addEventToTable(event, ui->eventTable);
    }
}

void MainWindow::on_addTime30M_clicked() {
    cur_datetime = cur_datetime.addSecs(30 * 60);
    ui->allEvents->setRowCount(0);
    for (int i = 0; i < 31; ++i) {
        for (auto& event : calendar[i].getEvents()) {
            addEventToTable(event, ui->allEvents);
        }
    }
    if (ui->tabWidget2->currentIndex() != 0) return;
    int i = ui->calendarWidget->selectedDate().day() - 1;
    ui->eventTable->setRowCount(0);
    for (auto& event : calendar[i].getEvents()) {
        addEventToTable(event, ui->eventTable);
    }
    ui->dateTime->setText(cur_datetime.toString());
}


void MainWindow::on_tabWidget_tabBarClicked(int index) {
    if (index != 0) return;
    ui->eventTable->setRowCount(0);
    int i = ui->calendarWidget->selectedDate().day() - 1;
    for (auto& event : calendar[i].getEvents()) {
        addEventToTable(event, ui->eventTable);
    }
}


void MainWindow::on_addTime1H_clicked() {
    cur_datetime = cur_datetime.addSecs(60 * 60);
    ui->allEvents->setRowCount(0);
    for (int i = 0; i < 31; ++i) {
        for (auto& event : calendar[i].getEvents()) {
            addEventToTable(event, ui->allEvents);
        }
    }
    if (ui->tabWidget2->currentIndex() != 0) return;
    int i = ui->calendarWidget->selectedDate().day() - 1;
    ui->eventTable->setRowCount(0);
    for (auto& event : calendar[i].getEvents()) {
        addEventToTable(event, ui->eventTable);
    }
    ui->dateTime->setText(cur_datetime.toString());
}

