// ============================================================
// Graph.h
// Adjacency-list Graph
//   Vertices = cities (array, looked up via AVLTree)
//   Edges    = linked list of Flight objects per vertex
//
// Features:
//   - Duplicate flight detection on load   (Rubric #4)
//   - BFS-based sub-graph generation       (Rubric #11)
// ============================================================
#ifndef GRAPH_H
#define GRAPH_H

#include "Flight.h"
#include "LinkedList.h"
#include "AVLTree.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// ---- One edge in the adjacency list ----
struct EdgeNode {
    Flight    flight;
    EdgeNode* next;
    EdgeNode(const Flight& f) : flight(f), next(nullptr) {}
};

// ---- City vertex ----
struct CityVertex {
    string    name;
    double    hotelPerDay;
    EdgeNode* flightHead;

    CityVertex() : hotelPerDay(0.0), flightHead(nullptr) {}
    ~CityVertex() {
        EdgeNode* cur = flightHead;
        while (cur) { EdgeNode* tmp = cur; cur = cur->next; delete tmp; }
    }

    void addFlight(const Flight& f) {
        EdgeNode* n = new EdgeNode(f);
        n->next = flightHead;
        flightHead = n;
    }
};

// ---- Graph ----
class Graph {
public:
    static const int MAX_CITIES = 100;

private:
    CityVertex cities[MAX_CITIES];
    int        cityCount;
    AVLTree    cityIndex;   // name -> array index
    AVLTree    hotelTree;   // name -> hotel price * 100 (cents)

    // --- Duplicate detection (Rubric #4) ---
    bool isDuplicateFlight(int cityIdx, const Flight& f) const {
        EdgeNode* cur = cities[cityIdx].flightHead;
        while (cur) {
            if (cur->flight == f) return true;
            cur = cur->next;
        }
        return false;
    }

public:
    Graph() : cityCount(0) {}

    int getOrAddCity(const string& name) {
        int idx = cityIndex.search(name);
        if (idx != -1) return idx;
        idx = cityCount++;
        cities[idx].name = name;
        cityIndex.insert(name, idx);
        return idx;
    }

    int getCityIndex(const string& name) const { return cityIndex.search(name); }

    const string& getCityName(int idx) const { return cities[idx].name; }
    int getCityCount() const { return cityCount; }

    void setHotelPrice(const string& city, double price) {
        int idx = getOrAddCity(city);
        cities[idx].hotelPerDay = price;
        hotelTree.insert(city, (int)(price * 100));
    }

    double getHotelPrice(const string& city) const {
        int val = hotelTree.search(city);
        return (val == -1) ? 0.0 : val / 100.0;
    }

    // Returns true if flight was added, false if duplicate
    bool addFlight(const Flight& f) {
        int idx = getOrAddCity(f.origin);
        getOrAddCity(f.destination);  // ensure destination vertex exists
        if (isDuplicateFlight(idx, f)) return false;
        cities[idx].addFlight(f);
        return true;
    }

    EdgeNode* getFlights(int cityIdx) const {
        if (cityIdx < 0 || cityIdx >= cityCount) return nullptr;
        return cities[cityIdx].flightHead;
    }

    // ---- Load Flights.txt (with duplicate detection) ----
    // Format per line: Origin Destination DD/MM/YYYY HH:MM HH:MM price Airline
    bool loadFlights(const string& filename) {
        ifstream fin(filename);
        if (!fin) { cout << "ERROR: Cannot open " << filename << "\n"; return false; }

        string origin, dest, dateStr, dep, arr, airline;
        double price;
        int count = 0, dupes = 0;

        while (fin >> origin >> dest >> dateStr >> dep >> arr >> price >> airline) {
            Flight f;
            f.origin = origin;
            f.destination = dest;
            f.date.parse(dateStr);
            f.depTime.parse(dep);
            f.arrTime.parse(arr);
            f.price = price;
            f.airline = airline;
            if (addFlight(f)) count++;
            else              dupes++;
        }
        fin.close();
        cout << "Loaded " << count << " flights from " << filename;
        if (dupes > 0) cout << " (" << dupes << " duplicates skipped)";
        cout << "\n";
        return true;
    }

    // ---- Load HotelCharges_perday.txt ----
    // Format per line: CityName price
    bool loadHotels(const string& filename) {
        ifstream fin(filename);
        if (!fin) { cout << "ERROR: Cannot open " << filename << "\n"; return false; }

        string city; double price;
        int count = 0;
        while (fin >> city >> price) {
            setHotelPrice(city, price);
            count++;
        }
        fin.close();
        cout << "Loaded " << count << " hotel entries from " << filename << "\n";
        return true;
    }

    void printAllCities() const {
        cout << "\n=== Cities in Graph (" << cityCount << ") ===\n";
        for (int i = 0; i < cityCount; i++)
            cout << "  [" << i << "] " << cities[i].name
            << "  Hotel/day: $" << cities[i].hotelPerDay << "\n";
    }

    // ============================================================
    // Efficient Sub-Graph Generation  (Rubric #11)
    //
    // BFS forward from source  -> set of reachable cities
    // BFS backward to dest     -> set of cities that can reach dest
    // Intersection = relevant sub-graph
    // ============================================================

    // BFS forward: mark all cities reachable from src
    void bfsForward(int src, bool* reachable) const {
        for (int i = 0; i < cityCount; i++) reachable[i] = false;
        reachable[src] = true;
        int queue[MAX_CITIES];
        int front = 0, back = 0;
        queue[back++] = src;
        while (front < back) {
            int cur = queue[front++];
            EdgeNode* edge = cities[cur].flightHead;
            while (edge) {
                int next = getCityIndex(edge->flight.destination);
                if (next != -1 && !reachable[next]) {
                    reachable[next] = true;
                    queue[back++] = next;
                }
                edge = edge->next;
            }
        }
    }

    // BFS backward: mark all cities that can reach dst (reverse edges)
    void bfsBackward(int dst, bool* reachable) const {
        for (int i = 0; i < cityCount; i++) reachable[i] = false;
        reachable[dst] = true;
        int queue[MAX_CITIES];
        int front = 0, back = 0;
        queue[back++] = dst;
        while (front < back) {
            int cur = queue[front++];
            for (int i = 0; i < cityCount; i++) {
                if (reachable[i]) continue;
                EdgeNode* edge = cities[i].flightHead;
                while (edge) {
                    int next = getCityIndex(edge->flight.destination);
                    if (next == cur) {
                        reachable[i] = true;
                        queue[back++] = i;
                        break;
                    }
                    edge = edge->next;
                }
            }
        }
    }

    // Returns count of relevant cities; fills filter[] for Dijkstra
    int generateSubGraphFilter(int src, int dst, bool* filter) const {
        bool forward[MAX_CITIES], backward[MAX_CITIES];
        bfsForward(src, forward);
        bfsBackward(dst, backward);
        int count = 0;
        for (int i = 0; i < cityCount; i++) {
            filter[i] = forward[i] && backward[i];
            if (filter[i]) count++;
        }
        filter[src] = true;
        filter[dst] = true;
        cout << "  Sub-graph: " << count << "/" << cityCount << " cities relevant\n";
        return count;
    }
};

#endif
