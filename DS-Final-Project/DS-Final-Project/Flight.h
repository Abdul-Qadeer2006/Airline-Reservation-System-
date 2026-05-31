// ============================================================
// Flight.h
// One flight segment — the data stored on each graph edge
// ============================================================
#ifndef FLIGHT_H
#define FLIGHT_H

#include "Date.h"
#include <string>
#include <iostream>
using namespace std;

struct Flight {
    string origin;
    string destination;
    Date   date;
    Time   depTime;
    Time   arrTime;
    double price;
    string airline;

    Flight() : price(0.0) {}

    bool operator==(const Flight& o) const {
        return origin == o.origin && destination == o.destination &&
            date == o.date && depTime == o.depTime && arrTime == o.arrTime &&
            price == o.price && airline == o.airline;
    }

    int durationMinutes() const { return flightDuration(depTime, arrTime); }

    void print() const {
        cout << "  [" << airline << "] "
            << origin << " -> " << destination
            << "  Date: "; date.print();
        cout << "  Dep: "; depTime.print();
        cout << "  Arr: "; arrTime.print();
        cout << "  Price: $" << price
            << "  Duration: " << durationMinutes() / 60
            << "h " << durationMinutes() % 60 << "m\n";
    }
};

#endif
