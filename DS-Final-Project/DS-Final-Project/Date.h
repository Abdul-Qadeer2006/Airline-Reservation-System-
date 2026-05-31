
#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>
using namespace std;

// Convert a string to an integer manually (no stoi)
inline int toInt(const string& s) {
    int result = 0;
    int start = 0;
    bool negative = false;
    if (s.length() > 0 && s[0] == '-') {
        negative = true;
        start = 1;
    }
    for (int i = start; i < (int)s.length(); i++) {
        if (s[i] < '0' || s[i] > '9') break;
        result = result * 10 + (s[i] - '0');
    }
    if (negative) result = -result;
    return result;
}

// Convert a string to a double manually (no stod)
inline double toDouble(const string& s) {
    double result = 0.0;
    int start = 0;
    bool negative = false;
    if (s.length() > 0 && s[0] == '-') {
        negative = true;
        start = 1;
    }
    // Integer part
    int i = start;
    for (; i < (int)s.length(); i++) {
        if (s[i] == '.') break;
        if (s[i] < '0' || s[i] > '9') break;
        result = result * 10 + (s[i] - '0');
    }
    // Decimal part
    if (i < (int)s.length() && s[i] == '.') {
        i++;
        double place = 0.1;
        for (; i < (int)s.length(); i++) {
            if (s[i] < '0' || s[i] > '9') break;
            result = result + (s[i] - '0') * place;
            place = place / 10.0;
        }
    }
    if (negative) result = -result;
    return result;
}

struct Date {
    int year, month, day;

    Date() : year(0), month(0), day(0) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    // Parse from "DD/MM/YYYY" (variable-width day/month)
    void parse(const string& s) {
        int slash1 = -1, slash2 = -1;
        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] == '/') {
                if (slash1 == -1) slash1 = i;
                else { slash2 = i; break; }
            }
        }
        day = toInt(s.substr(0, slash1));
        month = toInt(s.substr(slash1 + 1, slash2 - slash1 - 1));
        year = toInt(s.substr(slash2 + 1));
    }

    // Validate date string format "DD/MM/YYYY" and range
    static bool isValidDateStr(const string& s) {
        if (s.empty()) return false;
        // Find the two '/' separators
        int slash1 = -1, slash2 = -1;
        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] == '/') {
                if (slash1 == -1) slash1 = i;
                else if (slash2 == -1) slash2 = i;
                else return false; // more than 2 slashes
            }
        }
        if (slash1 == -1 || slash2 == -1) return false;
        // Extract parts
        string dayStr = s.substr(0, slash1);
        string monthStr = s.substr(slash1 + 1, slash2 - slash1 - 1);
        string yearStr = s.substr(slash2 + 1);
        if (dayStr.empty() || monthStr.empty() || yearStr.empty()) return false;
        // Check all digits
        for (int i = 0; i < (int)dayStr.length(); i++)
            if (dayStr[i] < '0' || dayStr[i] > '9') return false;
        for (int i = 0; i < (int)monthStr.length(); i++)
            if (monthStr[i] < '0' || monthStr[i] > '9') return false;
        for (int i = 0; i < (int)yearStr.length(); i++)
            if (yearStr[i] < '0' || yearStr[i] > '9') return false;
        int d = toInt(dayStr);
        int m = toInt(monthStr);
        int y = toInt(yearStr);
        if (m < 1 || m > 12) return false;
        int dpm[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
        if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) dpm[2] = 29;
        if (d < 1 || d > dpm[m]) return false;
        return true;
    }

    // Try to parse; returns false if invalid
    bool tryParse(const string& s) {
        if (!isValidDateStr(s)) return false;
        parse(s);
        return true;
    }

    bool operator==(const Date& o) const {
        return year == o.year && month == o.month && day == o.day;
    }
    bool operator<(const Date& o) const {
        if (year != o.year)  return year < o.year;
        if (month != o.month) return month < o.month;
        return day < o.day;
    }
    bool operator<=(const Date& o) const { return *this < o || *this == o; }
    bool operator>(const Date& o)  const { return o < *this; }

    // Approximate difference in days between two dates
    int diffDays(const Date& o) const {
        int days1 = year * 365 + month * 30 + day;
        int days2 = o.year * 365 + o.month * 30 + o.day;
        return days1 - days2;
    }

    void print() const {
        cout << day << "/" << month << "/" << year;
    }

    Date addDays(int delta) const {
        int dpm[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
        int d = day + delta, m = month, y = year;
        while (d > dpm[m]) { d -= dpm[m]; m++; if (m > 12) { m = 1; y++; } }
        while (d < 1) { m--;         if (m < 1) { m = 12; y--; } d += dpm[m]; }
        return Date(y, m, d);
    }
};

struct Time {
    int totalMinutes;

    Time() : totalMinutes(0) {}
    Time(int h, int m) : totalMinutes(h * 60 + m) {}

    // Parse from "H:MM" or "HH:MM" format
    void parse(const string& s) {
        // Find the ':' separator
        int colonPos = -1;
        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] == ':') { colonPos = i; break; }
        }
        int h = toInt(s.substr(0, colonPos));
        int m = toInt(s.substr(colonPos + 1));
        totalMinutes = h * 60 + m;
    }

    int hours()   const { return totalMinutes / 60; }
    int minutes() const { return totalMinutes % 60; }

    void print() const {
        int h = hours(), m = minutes();
        if (h < 10) cout << "0";
        cout << h << ":";
        if (m < 10) cout << "0";
        cout << m;
    }

    bool operator<(const Time& o)  const { return totalMinutes < o.totalMinutes; }
    bool operator<=(const Time& o) const { return totalMinutes <= o.totalMinutes; }
    bool operator==(const Time& o) const { return totalMinutes == o.totalMinutes; }
};

inline int flightDuration(const Time& dep, const Time& arr) {
    int diff = arr.totalMinutes - dep.totalMinutes;
    if (diff <= 0) diff += 24 * 60;
    return diff;
}

#endif
