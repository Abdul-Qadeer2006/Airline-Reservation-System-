// ============================================================
// Dijkstra.h
// Modified Dijkstra for the airline flight graph.
//
// State = (cityIdx, arrivalDate, arrivalTime, partialItinerary)
// This correctly handles multiple flights on the same route
// at different times, overnight layovers, and multi-day trips.
//
// Two modes:
//   MINIMIZE_COST — key = totalCost (tickets + hotel)
//   MINIMIZE_TIME — key = totalMinutes (flight + transit)
//
// Additional features:
//   - Sub-graph filter integration         (Rubric #11)
//   - Transit city via visitedTransit flag  (Rubric #7 fix)
//   - maxStops / maxBudget constraints      (Rubric #10)
// ============================================================
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include "Itinerary.h"
#include "MinHeap.h"
#include <string>
#include <iostream>
using namespace std;

enum CriterionType { MINIMIZE_COST, MINIMIZE_TIME };

// ---- Priority queue state ----
struct DijkState {
    double    key;       // cost or minutes — heap ordering key
    int       cityIdx;
    Date      curDate;
    Time      curTime;
    Itinerary itin;
    bool      visitedTransit;  // true once we've stopped at required transit city

    DijkState() : key(0), cityIdx(-1), visitedTransit(false) {}
};

// ---- Visited set: tracks (cityIdx, dateEncoded, timeMinutes) ----
struct VisitedEntry {
    int  cityIdx;
    int  dateEnc;    // year*10000 + month*100 + day
    int  timeMins;
};

class VisitedSet {
    static const int CAP = 4000;
    VisitedEntry entries[CAP];
    int sz;
public:
    VisitedSet() : sz(0) {}
    void reset() { sz = 0; }

    bool contains(int city, const Date& d, const Time& t) const {
        int de = d.year * 10000 + d.month * 100 + d.day;
        for (int i = 0; i < sz; i++)
            if (entries[i].cityIdx == city &&
                entries[i].dateEnc == de &&
                entries[i].timeMins == t.totalMinutes) return true;
        return false;
    }

    void insert(int city, const Date& d, const Time& t) {
        if (sz >= CAP) return;
        entries[sz++] = { city,
                          d.year * 10000 + d.month * 100 + d.day,
                          t.totalMinutes };
    }
};

// ============================================================
class DijkstraSolver {
    const Graph& graph;

    bool dateInWindow(const Date& flightDate, const Date& target) const {
        return (target.addDays(-1) <= flightDate && flightDate <= target.addDays(1));
    }

    bool canBoard(const Date& curDate, const Time& curTime,
        const Date& flightDate, const Time& depTime) const {
        if (flightDate == curDate)
            return depTime.totalMinutes > curTime.totalMinutes;
        return flightDate == curDate.addDays(1);
    }

    int computeTransit(const Date& arrDate, const Time& arrTime,
        const Date& depDate, const Time& depTime) const {
        int dayDiff = depDate.diffDays(arrDate);
        return dayDiff * 24 * 60 + (depTime.totalMinutes - arrTime.totalMinutes);
    }

    Date arrivalDate(const Date& depDate, const Time& dep, const Time& arr) const {
        return (arr.totalMinutes <= dep.totalMinutes)
            ? depDate.addDays(1) : depDate;
    }

public:
    DijkstraSolver(const Graph& g) : graph(g) {}

    // ---- Main search with all constraint parameters ----
    void search(
        const string& origin,
        const string& destination,
        const Date& targetDate,
        CriterionType criterion,
        ResultList& results,
        const string& preferredAirline = "",
        const string& transitCity = "",
        int           minTransitMins = -1,
        int           maxTransitMins = -1,
        bool          directOnly = false,
        int           maxResults = 10,
        const bool* cityFilter = nullptr,   // sub-graph filter (Rubric #11)
        int           maxStops = -1,         // max layovers     (Rubric #10)
        double        maxBudget = -1.0         // budget ceiling   (Rubric #10)
    ) const {
        int srcIdx = graph.getCityIndex(origin);
        int dstIdx = graph.getCityIndex(destination);

        if (srcIdx == -1) { cout << "  City not found: " << origin << "\n"; return; }
        if (dstIdx == -1) { cout << "  City not found: " << destination << "\n"; return; }

        MinHeap<DijkState> pq;
        VisitedSet         visited;

        // Seed: arrive at origin at end of day before target
        DijkState init;
        init.key = 0;
        init.cityIdx = srcIdx;
        init.curDate = targetDate.addDays(-1);
        init.curTime = Time(23, 59);
        init.visitedTransit = false;
        pq.insert(init);

        int found = 0;

        while (!pq.empty() && found < maxResults) {
            DijkState cur = pq.extractMin();

            // Reached destination — record result
            if (cur.cityIdx == dstIdx && cur.itin.numLegs() > 0) {
                // Transit city constraint: must have passed through it
                if (!transitCity.empty() && !cur.visitedTransit) {
                    continue;   // route did not visit required transit city
                }
                // Budget constraint
                if (maxBudget > 0 && cur.itin.totalCost > maxBudget) {
                    continue;
                }

                if (criterion == MINIMIZE_COST) results.addSorted_byCost(cur.itin);
                else                            results.addSorted_byTime(cur.itin);
                found++;
                continue;
            }

            if (visited.contains(cur.cityIdx, cur.curDate, cur.curTime)) continue;
            visited.insert(cur.cityIdx, cur.curDate, cur.curTime);

            EdgeNode* edge = graph.getFlights(cur.cityIdx);
            while (edge) {
                const Flight& f = edge->flight;

                // Airline filter
                if (!preferredAirline.empty() && f.airline != preferredAirline) {
                    edge = edge->next; continue;
                }

                // Direct-only: no connecting flights
                if (directOnly && cur.itin.numLegs() > 0) {
                    edge = edge->next; continue;
                }

                // Max stops constraint (Rubric #10)
                if (maxStops >= 0 && cur.itin.numLegs() >= maxStops + 1) {
                    edge = edge->next; continue;
                }

                // First leg must be within ±1 day of target date
                if (cur.itin.numLegs() == 0 && !dateInWindow(f.date, targetDate)) {
                    edge = edge->next; continue;
                }

                // Can we board?
                if (!canBoard(cur.curDate, cur.curTime, f.date, f.depTime)) {
                    edge = edge->next; continue;
                }

                int nextIdx = graph.getCityIndex(f.destination);
                if (nextIdx == -1) { edge = edge->next; continue; }

                // Prevent cycling back through origin (avoids routes like A->B->A->C)
                if (nextIdx == srcIdx && cur.itin.numLegs() > 0) {
                    edge = edge->next; continue;
                }

                // Sub-graph filter: skip cities not in relevant sub-graph (Rubric #11)
                if (cityFilter && !cityFilter[nextIdx]) {
                    edge = edge->next; continue;
                }

                // Compute transit time from last arrival to this flight
                int transit = 0;
                if (cur.itin.numLegs() > 0) {
                    transit = computeTransit(cur.curDate, cur.curTime,
                        f.date, f.depTime);

                    // Transit time constraints apply only at required transit city
                    if (!transitCity.empty() &&
                        graph.getCityName(cur.cityIdx) == transitCity) {
                        if (minTransitMins != -1 && transit < minTransitMins) {
                            edge = edge->next; continue;
                        }
                        if (maxTransitMins != -1 && transit > maxTransitMins) {
                            edge = edge->next; continue;
                        }
                    }
                    // Cap runaway layovers at 48 hours
                    if (transit > 48 * 60) { edge = edge->next; continue; }
                }

                // Build new partial itinerary
                Itinerary newItin = cur.itin;
                double hotelPrice = graph.getHotelPrice(graph.getCityName(cur.cityIdx));
                newItin.addLeg(f, hotelPrice, transit);

                // Budget pruning: skip if already over budget
                if (maxBudget > 0 && newItin.totalCost > maxBudget) {
                    edge = edge->next; continue;
                }

                Date newDate = arrivalDate(f.date, f.depTime, f.arrTime);
                Time newTime = f.arrTime;

                // Track transit city visitation
                bool nowVisited = cur.visitedTransit ||
                    (!transitCity.empty() && f.destination == transitCity);

                DijkState next;
                next.cityIdx = nextIdx;
                next.curDate = newDate;
                next.curTime = newTime;
                next.itin = newItin;
                next.visitedTransit = nowVisited;
                next.key = (criterion == MINIMIZE_COST)
                    ? newItin.totalCost
                    : (double)newItin.totalMinutes;

                pq.insert(next);
                edge = edge->next;
            }
        }
    }
};

#endif