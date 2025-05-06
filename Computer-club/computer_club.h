#ifndef COMPUTER_CLUB_H
#define COMPUTER_CLUB_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <queue>
#include <iostream>
#include <sstream>

class Time
{
private:
    int hours;
    int minutes;

public:
    Time() : hours(0), minutes(0) {}
    Time(int h, int m) : hours(h), minutes(m) {}

    static Time fromString(const std::string &str);
    int toMinutes() const;
    bool operator<(const Time &other) const;
    bool operator<=(const Time &other) const;
    bool operator>(const Time &other) const;
    bool operator>=(const Time &other) const;
    bool operator==(const Time &other) const;
    Time operator+(const Time &other) const;
    Time operator-(const Time &other) const;
    int getHours() const { return hours; };
    int getMinutes() const { return minutes; };
    std::string toString() const;
};

struct Table
{
    int number;
    int revenue = 0;
    Time occupied_time;
    Time start_time;
    bool is_occupied = false;
    std::string current_client;
};

struct Event
{
    Time time;
    int id;
    std::string client_name;
    int table_number = 0;
    std::string error_msg;

    std::string toString() const
    {
        std::ostringstream oss;
        oss << time.toString() << " " << id;
        if (!client_name.empty())
            oss << " " << client_name;
        if (table_number != 0)
            oss << " " << table_number;
        if (!error_msg.empty())
            oss << " " << error_msg;
        return oss.str();
    }
};

class ComputerClub
{
private:
    int table_count;
    Time open_time;
    Time close_time;
    int hour_cost;
    std::vector<Table> tables;
    std::set<std::string> clients_in_club;
    std::map<std::string, int> client_to_table;
    std::queue<std::string> waiting_queue;
    std::vector<Event> output_events;

    bool isClientInClub(const std::string &client_name) const;
    bool isTableOccupied(int table_number) const;
    int countFreeTables() const;
    void addRevenue(int table_number, const Time &start, const Time &end);
    void processClientSits(Event event);
    void processClientWaits(Event event);
    void processClientLeaves(Event event);

public:
    ComputerClub(int tables, Time open, Time close, int cost);
    void processEvent(Event event);
    void endOfDay();
    const std::vector<Event> &getOutputEvents() const;
    const std::vector<Table> &getTables() const;
    Time getOpenTime() const { return open_time; }
    Time getCloseTime() const { return close_time; }
};

void printUsage();

#endif // COMPUTER_CLUB_H