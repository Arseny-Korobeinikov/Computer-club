#include "computer_club.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

Time Time::fromString(const std::string &str)
{
    size_t colon_pos = str.find(':');
    if (colon_pos == std::string::npos || colon_pos == 0 || colon_pos == str.length() - 1)
    {
        throw std::invalid_argument("Invalid time format");
    }

    int h = std::stoi(str.substr(0, colon_pos));
    int m = std::stoi(str.substr(colon_pos + 1));

    if (h < 0 || h > 23 || m < 0 || m > 59)
    {
        throw std::invalid_argument("Invalid time value");
    }

    return Time(h, m);
}

int Time::toMinutes() const
{
    return hours * 60 + minutes;
}

bool Time::operator<(const Time &other) const
{
    return toMinutes() < other.toMinutes();
}

bool Time::operator<=(const Time &other) const
{
    return toMinutes() <= other.toMinutes();
}

bool Time::operator>(const Time &other) const
{
    return toMinutes() > other.toMinutes();
}

bool Time::operator>=(const Time &other) const
{
    return toMinutes() >= other.toMinutes();
}

bool Time::operator==(const Time &other) const
{
    return toMinutes() == other.toMinutes();
}

Time Time::operator+(const Time &other) const
{
    int total_minutes = toMinutes() + other.toMinutes();
    return Time(total_minutes / 60, total_minutes % 60);
}

Time Time::operator-(const Time &other) const
{
    int diff = toMinutes() - other.toMinutes();
    if (diff < 0)
        diff += 24 * 60;
    return Time(diff / 60, diff % 60);
}

std::string Time::toString() const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes;
    return oss.str();
}

ComputerClub::ComputerClub(int tables, Time open, Time close, int cost)
    : table_count(tables), open_time(open), close_time(close), hour_cost(cost)
{
    this->tables.resize(table_count);
    for (int i = 0; i < table_count; ++i)
    {
        this->tables[i].number = i + 1;
    }
}
bool ComputerClub::isClientInClub(const std::string &client_name) const
{
    return clients_in_club.find(client_name) != clients_in_club.end();
}

bool ComputerClub::isTableOccupied(int table_number) const
{
    return tables[table_number - 1].is_occupied;
}

int ComputerClub::countFreeTables() const
{
    int count = 0;
    for (const auto &table : tables)
    {
        if (!table.is_occupied)
        {
            count++;
        }
    }
    return count;
}

void ComputerClub::addRevenue(int table_number, const Time &start, const Time &end)
{
    int minutes = (end - start).toMinutes();
    int hours = (minutes + 59) / 60; // Округление вверх
    tables[table_number - 1].revenue += hours * hour_cost;
}

void ComputerClub::processClientSits(Event event)
{
    if (!isClientInClub(event.client_name))
    {
        output_events.push_back({event.time, 13, "", 0, "ClientUnknown"});
        return;
    }

    if (isTableOccupied(event.table_number))
    {
        output_events.push_back({event.time, 13, "", 0, "PlaceIsBusy"});
        return;
    }

    // Если клиент сидел за другим столом, освобождаем его и считаем выручку
    if (client_to_table.count(event.client_name))
    {
        int old_table = client_to_table[event.client_name];
        // Добавляем выручку за время занятия предыдущего стола
        addRevenue(old_table, tables[old_table - 1].start_time, event.time);
        // Обновляем общее время занятия стола
        tables[old_table - 1].occupied_time = tables[old_table - 1].occupied_time +
                                              (event.time - tables[old_table - 1].start_time);
        tables[old_table - 1].is_occupied = false;
    }
    // Занимаем новый стол
    tables[event.table_number - 1].is_occupied = true;
    tables[event.table_number - 1].current_client = event.client_name;
    tables[event.table_number - 1].start_time = event.time;
    client_to_table[event.client_name] = event.table_number;
}

void ComputerClub::processClientWaits(Event event)
{
    if (!isClientInClub(event.client_name))
    {
        output_events.push_back({event.time, 13, "", 0, "ClientUnknown"});
        return;
    }

    if (countFreeTables() > 0)
    {
        output_events.push_back({event.time, 13, "", 0, "ICanWaitNoLonger!"});
        return;
    }

    if (waiting_queue.size() >= table_count)
    {
        clients_in_club.erase(event.client_name);
        if (client_to_table.count(event.client_name))
        {
            int table_num = client_to_table[event.client_name];
            tables[table_num - 1].is_occupied = false;
            client_to_table.erase(event.client_name);
        }
        output_events.push_back({event.time, 11, event.client_name});
        return;
    }

    waiting_queue.push(event.client_name);
}

void ComputerClub::processClientLeaves(Event event)
{
    if (!isClientInClub(event.client_name))
    {
        output_events.push_back({event.time, 13, "", 0, "ClientUnknown"});
        return;
    }

    clients_in_club.erase(event.client_name);

    if (client_to_table.count(event.client_name))
    {
        int table_num = client_to_table[event.client_name];
        addRevenue(table_num, tables[table_num - 1].start_time, event.time);
        tables[table_num - 1].is_occupied = false;
        tables[table_num - 1].occupied_time = tables[table_num - 1].occupied_time +
                                              (event.time - tables[table_num - 1].start_time);
        client_to_table.erase(event.client_name);

        // Если есть ожидающие, сажаем первого в очереди
        if (!waiting_queue.empty())
        {
            std::string next_client = waiting_queue.front();
            waiting_queue.pop();
            tables[table_num - 1].is_occupied = true;
            tables[table_num - 1].current_client = next_client;
            tables[table_num - 1].start_time = event.time;
            client_to_table[next_client] = table_num;
            output_events.push_back({event.time, 12, next_client, table_num});
        }
    }
}

void ComputerClub::processEvent(Event event)
{
    output_events.push_back(event);

    switch (event.id)
    {
    case 1:
    { // Клиент пришел
        if (event.time < open_time || event.time >= close_time)
        {
            output_events.push_back({event.time, 13, "", 0, "NotOpenYet"});
            break;
        }
        if (isClientInClub(event.client_name))
        {
            output_events.push_back({event.time, 13, "", 0, "YouShallNotPass"});
            break;
        }
        clients_in_club.insert(event.client_name);
        break;
    }
    case 2: // Клиент сел за стол
        processClientSits(event);
        break;
    case 3: // Клиент ожидает
        processClientWaits(event);
        break;
    case 4: // Клиент ушел
        processClientLeaves(event);
        break;
    default:
        break;
    }
}

void ComputerClub::endOfDay()
{
    // Удаление клиентов в алфавитном порядке
    std::vector<std::string> remaining_clients(clients_in_club.begin(), clients_in_club.end());
    std::sort(remaining_clients.begin(), remaining_clients.end());

    for (const auto &client : remaining_clients)
    {
        output_events.push_back({close_time, 11, client});
        if (client_to_table.count(client))
        {
            int table_num = client_to_table[client];
            addRevenue(table_num, tables[table_num - 1].start_time, close_time);
            tables[table_num - 1].is_occupied = false;
            tables[table_num - 1].occupied_time = tables[table_num - 1].occupied_time +
                                                  (close_time - tables[table_num - 1].start_time);
        }
    }

    clients_in_club.clear();
    client_to_table.clear();
    while (!waiting_queue.empty())
        waiting_queue.pop();
}

const std::vector<Event> &ComputerClub::getOutputEvents() const
{
    return output_events;
}

const std::vector<Table> &ComputerClub::getTables() const
{
    return tables;
}

void printUsage()
{
    std::cerr << "Use: computer_club <input_file>\n";
}

int run_computer_club(const std::string &input_file_path, std::ostream &output_stream)
{
    std::ifstream input_file(input_file_path);
    if (!input_file.is_open())
    {
        output_stream << "Error: The file could not be opened\n";
        return 1;
    }

    try
    {
        // Чтение количества столов
        std::string line;
        if (!std::getline(input_file, line))
            throw std::runtime_error("Empty file");
        int table_count = std::stoi(line);
        if (table_count <= 0)
            throw std::runtime_error(line);

        // Чтение времени работы
        if (!std::getline(input_file, line))
            throw std::runtime_error(line);
        size_t space_pos = line.find(' ');
        if (space_pos == std::string::npos)
            throw std::runtime_error(line);

        Time open_time = Time::fromString(line.substr(0, space_pos));
        Time close_time = Time::fromString(line.substr(space_pos + 1));
        if (close_time <= open_time)
            throw std::runtime_error("The closing time must be after the opening time.");

        // Чтение стоимости часа
        if (!std::getline(input_file, line))
            throw std::runtime_error("Отсутствует стоимость часа");
        int hour_cost = std::stoi(line);
        if (hour_cost <= 0)
            throw std::runtime_error("Неверная стоимость часа");

        ComputerClub club(table_count, open_time, close_time, hour_cost);

        // Чтение событий
        while (std::getline(input_file, line))
        {
            if (line.empty())
                continue;

            std::istringstream iss(line);
            std::string time_str, id_str;
            if (!(iss >> time_str >> id_str))
                throw std::runtime_error("Неверный формат события: " + line);

            Time time = Time::fromString(time_str);
            int id = std::stoi(id_str);

            Event event;
            event.time = time;
            event.id = id;

            if (id == 1 || id == 3 || id == 4)
            {
                if (!(iss >> event.client_name))
                    throw std::runtime_error("Отсутствует имя клиента в событии: " + line);
            }
            else if (id == 2)
            {
                std::string table_str;
                if (!(iss >> event.client_name >> table_str))
                    throw std::runtime_error("Отсутствует имя клиента или номер стола в событии: " + line);
                event.table_number = std::stoi(table_str);
                if (event.table_number < 1 || event.table_number > table_count)
                {
                    throw std::runtime_error("Неверный номер стола в событии: " + line);
                }
            }
            else
            {
                throw std::runtime_error("Неизвестный ID события: " + line);
            }

            club.processEvent(event);
        }

        // Обработка конца дня
        club.endOfDay();

        // Вывод результатов
        output_stream << club.getOpenTime().toString() << "\n";
        for (const auto &event : club.getOutputEvents())
        {
            output_stream << event.toString() << "\n";
        }
        output_stream << club.getCloseTime().toString() << "\n";

        for (const auto &table : club.getTables())
        {
            output_stream << table.number << " " << table.revenue << " " << table.occupied_time.toString() << "\n";
        }
    }
    catch (const std::exception &e)
    {
        output_stream << e.what() << "\n";
        return 1;
    }

    return 0;
}

// int main(int argc, char *argv[])
//{
//     setlocale(LC_ALL, "Russian");
//     if (argc != 2)
//     {
//         printUsage();
//         return 1;
//     }
//
//     return run_computer_club(argv[1], std::cout);
// }