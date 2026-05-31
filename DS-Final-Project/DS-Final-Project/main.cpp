// ============================================================
// main.cpp
// Airline Reservation System ? Data Structures Project
// FAST NUCES ? Cyber Security ? Spring 2026
//
// Student Names  : [Your Names Here]
// Roll Numbers   : [Your Roll Numbers Here]
//
// Data Structures used:
//   Adjacency List Graph  ? cities as vertices, flights as edges
//   AVL BST               ? O(log n) city name lookup & hotel prices
//   Min-Heap              ? priority queue for Dijkstra's algorithm
//   Singly Linked List    ? adjacency lists, result lists, itinerary legs
//   Queue                 ? general-purpose FIFO (available utility)
//   Stack                 ? general-purpose LIFO (available utility)
//   Modified Dijkstra     ? minimise cost or travel time over flight graph
//   BFS Sub-graph Filter  ? prune irrelevant cities before Dijkstra
// ============================================================

#include "reservationsystem.h"
#include <string>
#include <iostream>
using namespace std;

// ---- Helpers ----

void sep() {
    cout << "------------------------------------------------------------\n";
}

void showMenu() {
    cout << "\n";
    sep();
    cout << "         AIRLINE RESERVATION SYSTEM\n";
    sep();
    cout << "  1. Scenario 1 : Cheapest flight  (specific airline)\n";
    cout << "  2. Scenario 2 : Fastest flight   (any airline)\n";
    cout << "  3. Scenario 3 : Via required transit city\n";
    cout << "  4. Scenario 4 : Transit city + duration constraint\n";
    cout << "  5. Scenario 5 : Direct flights only\n";
    cout << "  6. Scenario 6 : Budget-constrained fastest route\n";
    cout << "  7. Scenario 7 : Cheapest with max stops limit\n";
    cout << "  8. Show all cities\n";
    cout << "  0. Exit\n";
    sep();
    cout << "  Choice: ";
}

string readLine(const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// ---- Input Validation Helpers (Rubric #3) ----

// Trim leading/trailing whitespace
string trim(const string& s) {
    int start = 0, end = (int)s.length() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t')) start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t')) end--;
    return s.substr(start, end - start + 1);
}

// Check if string is a valid integer
bool isInteger(const string& s) {
    if (s.empty()) return false;
    int start = 0;
    if (s[0] == '-' || s[0] == '+') start = 1;
    if (start >= (int)s.length()) return false;
    for (int i = start; i < (int)s.length(); i++)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

// Check if string is a valid number (int or decimal)
bool isNumber(const string& s) {
    if (s.empty()) return false;
    int start = 0;
    bool hasDot = false;
    if (s[0] == '-' || s[0] == '+') start = 1;
    if (start >= (int)s.length()) return false;
    for (int i = start; i < (int)s.length(); i++) {
        if (s[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        }
        else if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

// Read a non-empty string with validation
string readNonEmpty(const string& prompt) {
    while (true) {
        string s = trim(readLine(prompt));
        if (!s.empty()) return s;
        cout << "  ** Input cannot be empty. Try again.\n";
    }
}

// Read and validate a city name that exists in the system
string readCity(const string& prompt, const ReservationSystem& rs) {
    while (true) {
        string city = readNonEmpty(prompt);
        if (rs.cityExists(city)) return city;
        cout << "  ** City '" << city << "' not found. Try again.\n";
    }
}

// Read and validate a date in DD/MM/YYYY format
Date readDate(const string& prompt) {
    while (true) {
        string s = readNonEmpty(prompt);
        Date d;
        if (d.tryParse(s)) return d;
        cout << "  ** Invalid date format. Use DD/MM/YYYY (e.g. 1/12/2019).\n";
    }
}

// Read and validate a positive integer
int readPositiveInt(const string& prompt) {
    while (true) {
        string s = readNonEmpty(prompt);
        if (isInteger(s)) {
            int val = toInt(s);
            if (val >= 0) return val;
        }
        cout << "  ** Please enter a valid non-negative integer.\n";
    }
}

// Read and validate a positive number (budget)
double readPositiveDouble(const string& prompt) {
    while (true) {
        string s = readNonEmpty(prompt);
        if (isNumber(s)) {
            double val = toDouble(s);
            if (val > 0) return val;
        }
        cout << "  ** Please enter a valid positive number.\n";
    }
}

// ---- Main ----

int main() {
    ReservationSystem rs;

    cout << "\n=== Loading airline data... ===\n";
    if (!rs.loadData("Flights.txt", "HotelCharges_perday.txt")) {
        cout << "Failed to load data files. Exiting.\n";
        return 1;
    }

    bool running = true;
    while (running) {
        showMenu();
        string choiceStr = trim(readLine());

        if (!isInteger(choiceStr)) {
            cout << "\n  ** Invalid choice. Enter a number 0-8.\n";
            continue;
        }
        int choice = toInt(choiceStr);
        cout << "\n";

        switch (choice) {

        case 1: {
            string origin = readCity("  Origin city       : ", rs);
            string dest = readCity("  Destination city  : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            string airline = readNonEmpty("  Preferred airline : ");
            rs.scenario1(origin, dest, date, airline);
            break;
        }

        case 2: {
            string origin = readCity("  Origin city      : ", rs);
            string dest = readCity("  Destination city : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            rs.scenario2(origin, dest, date);
            break;
        }

        case 3: {
            string origin = readCity("  Origin city        : ", rs);
            string dest = readCity("  Destination city   : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            string transit = readCity("  Required transit city: ", rs);
            rs.scenario3(origin, dest, date, transit);
            break;
        }

        case 4: {
            string origin = readCity("  Origin city        : ", rs);
            string dest = readCity("  Destination city   : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            string transit = readCity("  Required transit city: ", rs);
            int    minH = readPositiveInt("  Minimum transit hours : ");
            int    maxH = readPositiveInt("  Maximum transit hours : ");
            if (minH > maxH) {
                cout << "  ** Min cannot exceed max. Swapping.\n";
                int tmp = minH; minH = maxH; maxH = tmp;
            }
            rs.scenario4(origin, dest, date, transit, minH * 60, maxH * 60);
            break;
        }

        case 5: {
            string origin = readCity("  Origin city      : ", rs);
            string dest = readCity("  Destination city : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            rs.scenario5(origin, dest, date);
            break;
        }

        case 6: {
            string origin = readCity("  Origin city      : ", rs);
            string dest = readCity("  Destination city : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            double budget = readPositiveDouble("  Maximum budget ($): ");
            rs.scenario6(origin, dest, date, budget);
            break;
        }

        case 7: {
            string origin = readCity("  Origin city      : ", rs);
            string dest = readCity("  Destination city : ", rs);
            Date   date = readDate("  Travel date (DD/MM/YYYY): ");
            int    stops = readPositiveInt("  Max layovers (0=direct, 1=one stop, ...): ");
            rs.scenario7(origin, dest, date, stops);
            break;
        }

        case 8:
            rs.showCities();
            break;

        case 0:
            cout << "  Goodbye!\n";
            running = false;
            break;

        default:
            cout << "  ** Invalid choice. Please enter 0-8.\n";
        }
    }
    return 0;
}
