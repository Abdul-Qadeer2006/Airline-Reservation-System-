// ============================================================
// ReservationSystem.h
// High-level API — wraps Graph + DijkstraSolver
// Handles all 7 passenger query scenarios (5 provided + 2 extra)
//
// Integrates:
//   - Efficient sub-graph generation before each search (Rubric #11)
//   - Additional use cases: budget-limit & max-stops   (Rubric #10)
// ============================================================
#ifndef RESERVATIONSYSTEM_H
#define RESERVATIONSYSTEM_H

#include "Graph.h"
#include "Dijkstra.h"
#include "Itinerary.h"
#include <string>
#include <iostream>
using namespace std;

class ReservationSystem {
    Graph          graph;
    DijkstraSolver solver;

    void printHeader(const string& scenario) const {
        cout << "\n";
        cout << "============================================================\n";
        cout << "  " << scenario << "\n";
        cout << "============================================================\n";
    }

    // Generate sub-graph filter and print stats (Rubric #11)
    void buildFilter(const string& origin, const string& dest,
        bool* filter) const {
        int src = graph.getCityIndex(origin);
        int dst = graph.getCityIndex(dest);
        if (src == -1 || dst == -1) return;
        graph.generateSubGraphFilter(src, dst, filter);
    }

public:
    ReservationSystem() : solver(graph) {}

    bool loadData(const string& flightsFile, const string& hotelsFile) {
        return graph.loadFlights(flightsFile) & graph.loadHotels(hotelsFile);
    }

    void showCities() const { graph.printAllCities(); }

    // Check if a city exists in the graph
    bool cityExists(const string& name) const {
        return graph.getCityIndex(name) != -1;
    }

    // ----------------------------------------------------------
    // Scenario 1: Specific airline — minimize cost
    // ----------------------------------------------------------
    void scenario1(const string& origin, const string& dest,
        const Date& date, const string& airline) const {
        printHeader("Scenario 1: Specific Airline - Minimize Travel Cost");
        cout << "  From    : " << origin << "\n";
        cout << "  To      : " << dest << "\n";
        cout << "  Date    : "; date.print(); cout << "\n";
        cout << "  Airline : " << airline << "\n";

        bool filter[Graph::MAX_CITIES];
        buildFilter(origin, dest, filter);

        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_COST, results,
            airline, "", -1, -1, false, 10, filter);

        cout << "\n  Results (sorted by cost):\n";
        results.print();
    }

    // ----------------------------------------------------------
    // Scenario 2: Any airline — minimize travel time
    // ----------------------------------------------------------
    void scenario2(const string& origin, const string& dest,
        const Date& date) const {
        printHeader("Scenario 2: Any Airline - Minimize Travel Time");
        cout << "  From : " << origin << "\n";
        cout << "  To   : " << dest << "\n";
        cout << "  Date : "; date.print(); cout << "\n";

        bool filter[Graph::MAX_CITIES];
        buildFilter(origin, dest, filter);

        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_TIME, results,
            "", "", -1, -1, false, 10, filter);

        cout << "\n  Results (sorted by travel time):\n";
        results.print();
    }

    // ----------------------------------------------------------
    // Scenario 3: Required transit city
    // ----------------------------------------------------------
    void scenario3(const string& origin, const string& dest,
        const Date& date, const string& transitCity) const {
        printHeader("Scenario 3: Connecting Flight with Required Transit City");
        cout << "  From    : " << origin << "\n";
        cout << "  To      : " << dest << "\n";
        cout << "  Date    : "; date.print(); cout << "\n";
        cout << "  Transit : " << transitCity << "\n";

        // No sub-graph filter for transit scenario (transit city may be pruned)
        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_COST, results,
            "", transitCity, -1, -1, false, 10);

        cout << "\n  Results (sorted by cost):\n";
        results.print();
    }

    // ----------------------------------------------------------
    // Scenario 4: Transit city + min/max transit duration — minimize time
    // ----------------------------------------------------------
    void scenario4(const string& origin, const string& dest,
        const Date& date, const string& transitCity,
        int minTransitMins, int maxTransitMins) const {
        printHeader("Scenario 4: Transit Duration Constraint - Minimize Time");
        cout << "  From        : " << origin << "\n";
        cout << "  To          : " << dest << "\n";
        cout << "  Date        : "; date.print(); cout << "\n";
        cout << "  Transit     : " << transitCity << "\n";
        cout << "  Min transit : " << minTransitMins / 60 << "h\n";
        cout << "  Max transit : " << maxTransitMins / 60 << "h\n";

        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_TIME, results,
            "", transitCity, minTransitMins, maxTransitMins, false, 10);

        cout << "\n  Results (sorted by travel time):\n";
        results.print();
    }

    // ----------------------------------------------------------
    // Scenario 5: Direct flights only
    // ----------------------------------------------------------
    void scenario5(const string& origin, const string& dest,
        const Date& date) const {
        printHeader("Scenario 5: Direct Flights Only");
        cout << "  From : " << origin << "\n";
        cout << "  To   : " << dest << "\n";
        cout << "  Date : "; date.print(); cout << "\n";

        int srcIdx = graph.getCityIndex(origin);
        int dstIdx = graph.getCityIndex(dest);
        if (srcIdx == -1) { cout << "  City not found: " << origin << "\n"; return; }
        if (dstIdx == -1) { cout << "  City not found: " << dest << "\n"; return; }

        ResultList results;
        EdgeNode* edge = graph.getFlights(srcIdx);
        bool       any = false;

        while (edge) {
            const Flight& f = edge->flight;
            if (f.destination == dest && f.date == date) {
                Itinerary itin;
                itin.addLeg(f, 0.0, 0);
                results.addSorted_byCost(itin);
                any = true;
            }
            edge = edge->next;
        }

        cout << "\n  Direct flights:\n";
        if (!any) cout << "  No direct flights found.\n";
        else      results.print();
    }

    // ----------------------------------------------------------
    // Scenario 6 (Additional): Budget-Constrained Fastest Route
    //   Find the fastest route whose total cost <= budget
    // ----------------------------------------------------------
    void scenario6(const string& origin, const string& dest,
        const Date& date, double maxBudget) const {
        printHeader("Scenario 6: Budget-Constrained Fastest Route");
        cout << "  From   : " << origin << "\n";
        cout << "  To     : " << dest << "\n";
        cout << "  Date   : "; date.print(); cout << "\n";
        cout << "  Budget : $" << maxBudget << "\n";

        bool filter[Graph::MAX_CITIES];
        buildFilter(origin, dest, filter);

        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_TIME, results,
            "", "", -1, -1, false, 10, filter, -1, maxBudget);

        cout << "\n  Results (fastest under budget, sorted by time):\n";
        results.print();
    }

    // ----------------------------------------------------------
    // Scenario 7 (Additional): Maximum Stops Constraint
    //   Find cheapest route with at most N layovers
    // ----------------------------------------------------------
    void scenario7(const string& origin, const string& dest,
        const Date& date, int maxStops) const {
        printHeader("Scenario 7: Maximum Stops Constraint");
        cout << "  From      : " << origin << "\n";
        cout << "  To        : " << dest << "\n";
        cout << "  Date      : "; date.print(); cout << "\n";
        cout << "  Max stops : " << maxStops << "\n";

        bool filter[Graph::MAX_CITIES];
        buildFilter(origin, dest, filter);

        ResultList results;
        solver.search(origin, dest, date, MINIMIZE_COST, results,
            "", "", -1, -1, false, 10, filter, maxStops);

        cout << "\n  Results (cheapest with max " << maxStops << " stop(s), sorted by cost):\n";
        results.print();
    }
};

#endif