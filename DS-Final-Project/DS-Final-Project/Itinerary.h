// ============================================================
// Itinerary.h
// A complete travel option (1+ legs) with cost & time totals
// ============================================================
#ifndef ITINERARY_H
#define ITINERARY_H

#include "Flight.h"
#include "LinkedList.h"
#include <iostream>
using namespace std;

struct Itinerary {
    LinkedList<Flight> legs;
    double totalCost;
    int    totalMinutes;
    int    transitMinutesTotal;

    Itinerary() : totalCost(0.0), totalMinutes(0), transitMinutesTotal(0) {}

    void addLeg(const Flight& f, double hotelPriceAtTransit = 0.0,
        int transitMins = 0) {
        legs.pushBack(f);
        totalCost += f.price;
        totalMinutes += f.durationMinutes();

        if (transitMins > 0) {
            totalMinutes += transitMins;
            transitMinutesTotal += transitMins;
            if (transitMins > 12 * 60) {
                double days = transitMins / (24.0 * 60.0);
                totalCost += days * hotelPriceAtTransit;
            }
        }
    }

    int numLegs() const { return legs.size(); }

    void print() const {
        cout << "  Legs: " << numLegs()
            << "  Total Cost: $" << totalCost
            << "  Total Time: " << totalMinutes / 60 << "h "
            << totalMinutes % 60 << "m";
        if (transitMinutesTotal > 0)
            cout << "  (Transit: " << transitMinutesTotal / 60 << "h "
            << transitMinutesTotal % 60 << "m)";
        cout << "\n";
        int legNo = 1;
        LLNode<Flight>* cur = legs.head;
        while (cur) {
            cout << "    Leg " << legNo++ << ": ";
            cur->data.print();
            cur = cur->next;
        }
    }
};

// ---- Sorted result list ----
struct ResultNode {
    Itinerary   itin;
    ResultNode* next;
    ResultNode(const Itinerary& it) : itin(it), next(nullptr) {}
};

class ResultList {
public:
    ResultNode* head;
    int         sz;

    ResultList() : head(nullptr), sz(0) {}
    ~ResultList() { clear(); }

    void addSorted_byCost(const Itinerary& it) {
        ResultNode* n = new ResultNode(it);
        if (!head || it.totalCost < head->itin.totalCost) {
            n->next = head; head = n; sz++; return;
        }
        ResultNode* cur = head;
        while (cur->next && cur->next->itin.totalCost <= it.totalCost)
            cur = cur->next;
        n->next = cur->next; cur->next = n; sz++;
    }

    void addSorted_byTime(const Itinerary& it) {
        ResultNode* n = new ResultNode(it);
        if (!head || it.totalMinutes < head->itin.totalMinutes) {
            n->next = head; head = n; sz++; return;
        }
        ResultNode* cur = head;
        while (cur->next && cur->next->itin.totalMinutes <= it.totalMinutes)
            cur = cur->next;
        n->next = cur->next; cur->next = n; sz++;
    }

    void clear() {
        while (head) { ResultNode* tmp = head; head = head->next; delete tmp; }
        sz = 0;
    }

    bool empty() const { return head == nullptr; }
    int  size()  const { return sz; }

    void print() const {
        if (!head) { cout << "  No options found.\n"; return; }
        ResultNode* cur = head;
        int rank = 1;
        while (cur) {
            cout << "\n  Option " << rank++ << ":\n";
            cur->itin.print();
            cur = cur->next;
        }
    }
};

#endif
