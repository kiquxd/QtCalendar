#include "editevent.h"
#include "ui_editevent.h"
#include "error.h"
#include <iostream>

EditEvent::EditEvent(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditEvent) {
    ui->setupUi(this);
}

EditEvent::~EditEvent() {
    delete ui;
}

void EditEvent::slotEditButton(Event& event, MainWindow* _par) {
    par = _par;
    ui->Name->setText(event.getName());
    ui->Place->setText(event.getPlace());
    ui->startTime->setDateTime(event.getStartDate());
    ui->endTime->setDateTime(event.getEndDate());
    for (auto& id : event.getMembers()) {
        ui->Participants->addItem(QString::number(id));
    }
}

void EditEvent::on_DeleteButton_clicked() {
    if (ui->Participants->currentItem() == nullptr) return;
    delete ui->Participants->currentItem();
}

void EditEvent::on_AddButton_clicked() {
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

void EditEvent::throwError(QString msg) {
    Error* w = new Error(nullptr, msg);
    w->show();
}

void EditEvent::on_OKbutton_clicked() {
    QVector<int> ids;
    QDateTime startDate = ui->startTime->dateTime();
    QDateTime endDate = ui->endTime->dateTime();
    QString place = ui->Place->text();
    QString name = ui->Name->text();
    if (place.size() == 0 || name.size() == 0) {
        throwError("Не заполнено место или время");
        return;
    }
    for (auto& [stTime, eTime] : par->places[place]) {
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
        if (par->employers.find(id) == par->employers.end()) {
            par->employers[id] = Employer(id);
        }
        for (auto& event : par->employers[id].getEvents()) {
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
    connect(this, SIGNAL(signalEditFinished(Event&)), par, SLOT(slotEditFinished(Event&)));
    close();
    emit signalEditFinished(event);
}
