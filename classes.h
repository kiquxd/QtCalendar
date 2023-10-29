#ifndef CLASSES_H
#define CLASSES_H

#include <QVector>
#include <QString>
#include <QTime>
class Event {
protected:
    QString name;
    QDateTime startDate;
    QDateTime endDate;
    QString place;
    QVector<int> participants;
public:
    Event(QString name, QDateTime startDate, QDateTime endDate, QString place, QVector<int> participants)
        : name(name), startDate(startDate), endDate(endDate), place(place), participants(participants) {
    }
    bool operator ==(const Event& ev) const {
        return name == ev.getName() && startDate == ev.getStartDate() && endDate == ev.getEndDate() && place == ev.getPlace();
    }
    Event() {}
    QString getName() const {
        return name;
    }
    QDateTime getStartDate() const {
        return startDate;
    }
    QDateTime getEndDate() const {
        return endDate;
    }
    QString getPlace() const {
        return place;
    }
    QVector<int> getMembers() const {
        return participants;
    }
};

class Employer {
protected:
    int id;
    QVector<Event> events;
public:
    Employer() {}
    Employer(int id) : id(id) {}
    void addEvent(Event& ev) {
        events.push_back(ev);
    }
    QVector<Event> getEvents() {
        return events;
    }
    void eraseEvent(Event& event) {
        if (!events.contains(event)) return;
        events.erase(events.begin() + events.indexOf(event));
    }
};

class Day {
protected:
    QVector<Event> events;
public:
    Day() {}
    void addEvent(Event& ev) {
        events.push_back(ev);
    }
    QVector<Event> getEvents() {
        return events;
    }
    void eraseEvent(Event& event) {
        if (!events.contains(event)) return;
        events.erase(events.begin() + (int)events.indexOf(event));
    }
};

#endif // CLASSES_H
