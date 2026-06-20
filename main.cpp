#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

enum class EventStatus {
    Planned = 1,
    InProgress = 2,
    Done = 3,
    Cancelled = 4
};

enum class TaskStatus {
    Todo = 1,
    Doing = 2,
    Done = 3
};

struct Participant {
    int id{};
    string firstName;
    string lastName;
    string email;
    bool paid{};
};

struct Task {
    int id{};
    string title;
    string owner;
    string deadline;
    TaskStatus status{TaskStatus::Todo};
};

struct BudgetItem {
    int id{};
    string name;
    double amount{};
    bool income{};
};

struct Event {
    int id{};
    string name;
    string date;
    string location;
    string description;
    EventStatus status{EventStatus::Planned};
    vector<Participant> participants;
    vector<Task> tasks;
    vector<BudgetItem> budget;
};

static const string DatabaseFileName = "wydarzenia.md";

namespace ui {
    const string Reset = "\033[0m";
    const string Muted = "\033[90m";
    const string Accent = "\033[36m";
    const string Green = "\033[32m";
    const string Yellow = "\033[33m";
    const string Red = "\033[31m";
    const string Bold = "\033[1m";

    void clear() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void line(int width = 78) {
        cout << Muted << string(width, '-') << Reset << '\n';
    }

    string fitText(const string& text, int width) {
        if (static_cast<int>(text.size()) <= width) {
            return text + string(width - text.size(), ' ');
        }
        if (width <= 3) {
            return text.substr(0, width);
        }
        return text.substr(0, width - 3) + "...";
    }

    void frameLine(int width = 78) {
        cout << Accent << '+' << string(width - 2, '-') << '+' << Reset << '\n';
    }

    void title(const string& text) {
        const int width = 78;
        cout << '\n';
        frameLine(width);
        cout << Accent << '|' << Reset << Bold << ' ' << fitText(text, width - 4) << ' ' << Reset << Accent << '|' << Reset << '\n';
        frameLine(width);
    }

    void section(const string& text) {
        const int width = 52;
        cout << '\n';
        frameLine(width);
        cout << Accent << '|' << Reset << Bold << ' ' << fitText(text, width - 4) << ' ' << Reset << Accent << '|' << Reset << '\n';
        frameLine(width);
    }

    void menuItem(int number, const string& label) {
        const int width = 78;
        string numberText = number < 10 ? " " + to_string(number) : to_string(number);
        string row = " " + numberText + "  " + label;
        cout << Accent << '|' << Reset << fitText(row, width - 2) << Accent << '|' << Reset << '\n';
    }

    void menuExit(const string& label) {
        const int width = 78;
        string row = "  0  " + label;
        cout << Accent << '|' << Reset << fitText(row, width - 2) << Accent << '|' << Reset << '\n';
        frameLine(width);
    }

    void success(const string& message) {
        cout << Green << "[OK] " << Reset << message << '\n';
    }

    void warning(const string& message) {
        cout << Yellow << "[!] " << Reset << message << '\n';
    }

    void error(const string& message) {
        cout << Red << "[X] " << Reset << message << '\n';
    }

    void empty(const string& message) {
        cout << Muted << message << Reset << '\n';
    }

    void pause() {
        cout << '\n' << Muted << "Nacisnij Enter, aby kontynuowac..." << Reset;
        string ignored;
        getline(cin, ignored);
    }
}

string trim(const string& value) {
    const auto first = find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return isspace(c);
    });
    const auto last = find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return isspace(c);
    }).base();

    if (first >= last) {
        return "";
    }
    return string(first, last);
}

string toLower(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });
    return value;
}

string readLine(const string& prompt) {
    cout << ui::Accent << "> " << ui::Reset << prompt;
    string value;
    if (!getline(cin, value)) {
        cout << '\n';
        ui::error("Przerwano wejscie programu.");
        exit(1);
    }
    return trim(value);
}

string readRequiredLine(const string& prompt) {
    while (true) {
        string value = readLine(prompt);
        if (!value.empty()) {
            return value;
        }
        ui::warning("To pole nie moze byc puste.");
    }
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool isValidDate(const string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
        return false;
    }

    for (size_t i = 0; i < value.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (!isdigit(static_cast<unsigned char>(value[i]))) {
            return false;
        }
    }

    int year = stoi(value.substr(0, 4));
    int month = stoi(value.substr(5, 2));
    int day = stoi(value.substr(8, 2));

    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return day <= 29;
    }
    return day <= daysInMonth[month - 1];
}

string readDate(const string& prompt) {
    while (true) {
        string value = readRequiredLine(prompt);
        if (isValidDate(value)) {
            return value;
        }
        ui::warning("Data musi byc w poprawnym formacie RRRR-MM-DD, np. 2026-08-03.");
    }
}

int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        cout << ui::Accent << "> " << ui::Reset << prompt;
        string line;
        if (!getline(cin, line)) {
            cout << '\n';
            ui::error("Przerwano wejscie programu.");
            exit(1);
        }
        stringstream stream(line);
        int value{};
        if (stream >> value && value >= minValue && value <= maxValue) {
            return value;
        }
        ui::warning("Niepoprawna wartosc. Podaj liczbe od " + to_string(minValue) + " do " + to_string(maxValue) + ".");
    }
}

double readDouble(const string& prompt) {
    while (true) {
        cout << ui::Accent << "> " << ui::Reset << prompt;
        string line;
        if (!getline(cin, line)) {
            cout << '\n';
            ui::error("Przerwano wejscie programu.");
            exit(1);
        }
        replace(line.begin(), line.end(), ',', '.');
        stringstream stream(line);
        double value{};
        if (stream >> value) {
            return value;
        }
        ui::warning("Niepoprawna kwota.");
    }
}

double readPositiveDouble(const string& prompt) {
    while (true) {
        double value = readDouble(prompt);
        if (value > 0.0) {
            return value;
        }
        ui::warning("Kwota musi byc wieksza od zera.");
    }
}

bool readYesNo(const string& prompt) {
    while (true) {
        string value = toLower(readLine(prompt + " (t/n): "));
        if (value == "t" || value == "tak" || value == "y" || value == "yes") {
            return true;
        }
        if (value == "n" || value == "nie" || value == "no") {
            return false;
        }
        ui::warning("Wpisz t albo n.");
    }
}

string statusToString(EventStatus status) {
    switch (status) {
        case EventStatus::Planned: return "Planowane";
        case EventStatus::InProgress: return "W trakcie";
        case EventStatus::Done: return "Zakonczone";
        case EventStatus::Cancelled: return "Anulowane";
    }
    return "Nieznane";
}

string statusToString(TaskStatus status) {
    switch (status) {
        case TaskStatus::Todo: return "Do zrobienia";
        case TaskStatus::Doing: return "W trakcie";
        case TaskStatus::Done: return "Gotowe";
    }
    return "Nieznane";
}

bool isValidEventStatus(int status) {
    return status >= static_cast<int>(EventStatus::Planned) && status <= static_cast<int>(EventStatus::Cancelled);
}

bool isValidTaskStatus(int status) {
    return status >= static_cast<int>(TaskStatus::Todo) && status <= static_cast<int>(TaskStatus::Done);
}

EventStatus chooseEventStatus() {
    ui::section("Status wydarzenia");
    ui::menuItem(1, "Planowane");
    ui::menuItem(2, "W trakcie");
    ui::menuItem(3, "Zakonczone");
    ui::menuItem(4, "Anulowane");
    return static_cast<EventStatus>(readInt("Status: ", 1, 4));
}

TaskStatus chooseTaskStatus() {
    ui::section("Status zadania");
    ui::menuItem(1, "Do zrobienia");
    ui::menuItem(2, "W trakcie");
    ui::menuItem(3, "Gotowe");
    return static_cast<TaskStatus>(readInt("Status zadania: ", 1, 3));
}

int nextEventId(const vector<Event>& events) {
    int maxId = 0;
    for (const auto& event : events) {
        maxId = max(maxId, event.id);
    }
    return maxId + 1;
}

template <typename T>
int nextNestedId(const vector<T>& items) {
    int maxId = 0;
    for (const auto& item : items) {
        maxId = max(maxId, item.id);
    }
    return maxId + 1;
}

Event* findEvent(vector<Event>& events, int id) {
    auto it = find_if(events.begin(), events.end(), [id](const Event& event) {
        return event.id == id;
    });
    return it == events.end() ? nullptr : &(*it);
}

const Event* findEvent(const vector<Event>& events, int id) {
    auto it = find_if(events.begin(), events.end(), [id](const Event& event) {
        return event.id == id;
    });
    return it == events.end() ? nullptr : &(*it);
}

fs::path databasePath(const string& programPath) {
    fs::path executablePath = fs::absolute(programPath);
    fs::path directory = executablePath.has_parent_path() ? executablePath.parent_path() : fs::current_path();
    return directory / DatabaseFileName;
}

string encodeField(const string& value) {
    string encoded;
    for (char c : value) {
        if (c == '\\') {
            encoded += "\\\\";
        } else if (c == '|') {
            encoded += "\\p";
        } else if (c == '\n') {
            encoded += "\\n";
        } else {
            encoded += c;
        }
    }
    return encoded;
}

string decodeField(const string& value) {
    string decoded;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            char next = value[++i];
            if (next == 'p') {
                decoded += '|';
            } else if (next == 'n') {
                decoded += '\n';
            } else {
                decoded += next;
            }
        } else {
            decoded += value[i];
        }
    }
    return decoded;
}

vector<string> splitRecord(const string& line) {
    vector<string> parts;
    string current;
    bool escaped = false;

    for (char c : line) {
        if (escaped) {
            current += '\\';
            current += c;
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '|') {
            parts.push_back(decodeField(current));
            current.clear();
        } else {
            current += c;
        }
    }

    if (escaped) {
        current += '\\';
    }
    parts.push_back(decodeField(current));
    return parts;
}

void saveEvents(const vector<Event>& events, const fs::path& path) {
    ofstream file(path);
    if (!file) {
        ui::error("Nie mozna zapisac pliku bazy danych.");
        return;
    }

    file << "# Wydarzenia\n\n";
    file << "Ten plik jest tworzony i aktualizowany automatycznie przez program.\n";
    file << "Mozesz go czytac jako raport Markdown, ale nie usuwaj bloku `event-manager-data`.\n\n";

    if (events.empty()) {
        file << "_Brak zapisanych wydarzen._\n\n";
    }

    for (const auto& event : events) {
        int paid = count_if(event.participants.begin(), event.participants.end(), [](const Participant& participant) {
            return participant.paid;
        });
        int doneTasks = count_if(event.tasks.begin(), event.tasks.end(), [](const Task& task) {
            return task.status == TaskStatus::Done;
        });
        double balance = 0.0;
        for (const auto& item : event.budget) {
            balance += item.income ? item.amount : -item.amount;
        }

        file << "## " << event.name << "\n\n";
        file << "- ID: " << event.id << "\n";
        file << "- Data: " << event.date << "\n";
        file << "- Miejsce: " << event.location << "\n";
        file << "- Status: " << statusToString(event.status) << "\n";
        file << "- Uczestnicy: " << event.participants.size() << " (oplaceni: " << paid << ")\n";
        file << "- Zadania: " << event.tasks.size() << " (zakonczone: " << doneTasks << ")\n";
        file << "- Bilans: " << fixed << setprecision(2) << balance << " PLN\n\n";
        if (!event.description.empty()) {
            file << event.description << "\n\n";
        }
    }

    file << "```event-manager-data\n";
    for (const auto& event : events) {
        file << "EVENT|" << event.id << '|'
             << encodeField(event.name) << '|'
             << encodeField(event.date) << '|'
             << encodeField(event.location) << '|'
             << encodeField(event.description) << '|'
             << static_cast<int>(event.status) << '\n';

        for (const auto& participant : event.participants) {
            file << "PARTICIPANT|" << event.id << '|'
                 << participant.id << '|'
                 << encodeField(participant.firstName) << '|'
                 << encodeField(participant.lastName) << '|'
                 << encodeField(participant.email) << '|'
                 << participant.paid << '\n';
        }

        for (const auto& task : event.tasks) {
            file << "TASK|" << event.id << '|'
                 << task.id << '|'
                 << encodeField(task.title) << '|'
                 << encodeField(task.owner) << '|'
                 << encodeField(task.deadline) << '|'
                 << static_cast<int>(task.status) << '\n';
        }

        for (const auto& item : event.budget) {
            file << "BUDGET|" << event.id << '|'
                 << item.id << '|'
                 << encodeField(item.name) << '|'
                 << fixed << setprecision(2) << item.amount << '|'
                 << item.income << '\n';
        }
    }
    file << "```\n";
}

void loadEventRecord(vector<Event>& events, const string& line) {
    vector<string> parts = splitRecord(line);
    if (parts.empty()) {
        return;
    }

    try {
        if (parts[0] == "EVENT" && parts.size() == 7) {
            int status = stoi(parts[6]);
            if (!isValidEventStatus(status)) {
                throw runtime_error("invalid event status");
            }
            Event event;
            event.id = stoi(parts[1]);
            event.name = parts[2];
            event.date = parts[3];
            event.location = parts[4];
            event.description = parts[5];
            event.status = static_cast<EventStatus>(status);
            events.push_back(event);
        } else if (parts[0] == "PARTICIPANT" && parts.size() == 7) {
            Event* event = findEvent(events, stoi(parts[1]));
            if (event != nullptr) {
                event->participants.push_back({
                    stoi(parts[2]), parts[3], parts[4], parts[5], stoi(parts[6]) != 0
                });
            }
        } else if (parts[0] == "TASK" && parts.size() == 7) {
            int status = stoi(parts[6]);
            if (!isValidTaskStatus(status)) {
                throw runtime_error("invalid task status");
            }
            Event* event = findEvent(events, stoi(parts[1]));
            if (event != nullptr) {
                event->tasks.push_back({
                    stoi(parts[2]), parts[3], parts[4], parts[5],
                    static_cast<TaskStatus>(status)
                });
            }
        } else if (parts[0] == "BUDGET" && parts.size() == 6) {
            Event* event = findEvent(events, stoi(parts[1]));
            if (event != nullptr) {
                event->budget.push_back({
                    stoi(parts[2]), parts[3], stod(parts[4]), stoi(parts[5]) != 0
                });
            }
        }
    } catch (...) {
        ui::warning("Pominieto uszkodzony rekord w bazie: " + line);
    }
}

vector<Event> loadLegacyEvents(const fs::path& path) {
    vector<Event> events;
    ifstream file(path);
    if (!file) {
        return events;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            loadEventRecord(events, line);
        }
    }
    return events;
}

vector<Event> loadEvents(const fs::path& path) {
    vector<Event> events;
    ifstream file(path);
    if (!file) {
        return events;
    }

    string line;
    bool inDataBlock = false;
    while (getline(file, line)) {
        if (line == "```event-manager-data") {
            inDataBlock = true;
            continue;
        }
        if (inDataBlock && line == "```") {
            break;
        }
        if (!inDataBlock || line.empty()) {
            continue;
        }
        loadEventRecord(events, line);
    }
    return events;
}

void printEventRow(const Event& event) {
    cout << " " << setw(4) << right << event.id << "  "
         << setw(26) << left << event.name.substr(0, 26)
         << setw(13) << event.date.substr(0, 10)
         << setw(20) << event.location.substr(0, 20)
         << statusToString(event.status) << right << '\n';
}

void listEvents(const vector<Event>& events) {
    if (events.empty()) {
        ui::empty("Brak wydarzen.");
        return;
    }

    cout << ui::Muted
         << "   ID  Nazwa                     Data         Miejsce             Status\n"
         << "  --------------------------------------------------------------------------"
         << ui::Reset << '\n';
    for (const auto& event : events) {
        printEventRow(event);
    }
}

int chooseEventId(const vector<Event>& events) {
    listEvents(events);
    if (events.empty()) {
        return -1;
    }
    return readInt("ID wydarzenia: ", 1, numeric_limits<int>::max());
}

void addEvent(vector<Event>& events) {
    ui::section("Nowe wydarzenie");
    Event event;
    event.id = nextEventId(events);
    event.name = readRequiredLine("Nazwa: ");
    event.date = readDate("Data (RRRR-MM-DD): ");
    event.location = readRequiredLine("Miejsce: ");
    event.description = readLine("Opis: ");
    event.status = chooseEventStatus();
    events.push_back(event);
    ui::success("Dodano wydarzenie.");
}

void editEvent(vector<Event>& events) {
    int id = chooseEventId(events);
    Event* event = findEvent(events, id);
    if (event == nullptr) {
        ui::error("Nie znaleziono wydarzenia.");
        return;
    }

    ui::section("Edycja wydarzenia");
    ui::empty("Zostaw puste pole, aby zachowac obecna wartosc.");
    string value = readLine("Nazwa [" + event->name + "]: ");
    if (!value.empty()) event->name = value;

    while (true) {
        value = readLine("Data [" + event->date + "]: ");
        if (value.empty()) {
            break;
        }
        if (isValidDate(value)) {
            event->date = value;
            break;
        }
        ui::warning("Data musi byc w poprawnym formacie RRRR-MM-DD, np. 2026-08-03.");
    }

    value = readLine("Miejsce [" + event->location + "]: ");
    if (!value.empty()) event->location = value;

    value = readLine("Opis [" + event->description + "]: ");
    if (!value.empty()) event->description = value;

    if (readYesNo("Zmienic status?")) {
        event->status = chooseEventStatus();
    }
    ui::success("Zaktualizowano wydarzenie.");
}

void deleteEvent(vector<Event>& events) {
    int id = chooseEventId(events);
    auto it = find_if(events.begin(), events.end(), [id](const Event& event) {
        return event.id == id;
    });

    if (it == events.end()) {
        ui::error("Nie znaleziono wydarzenia.");
        return;
    }

    if (!readYesNo("Usunac wydarzenie \"" + it->name + "\"?")) {
        ui::empty("Usuwanie anulowane.");
        return;
    }

    events.erase(it);
    ui::success("Usunieto wydarzenie.");
}

void searchEvents(const vector<Event>& events) {
    ui::section("Wyszukiwarka");
    string query = toLower(readRequiredLine("Szukaj w nazwie, miejscu lub opisie: "));
    bool found = false;
    for (const auto& event : events) {
        string haystack = toLower(event.name + " " + event.location + " " + event.description);
        if (haystack.find(query) != string::npos) {
            if (!found) {
                ui::section("Wyniki");
            }
            printEventRow(event);
            found = true;
        }
    }
    if (!found) {
        ui::empty("Brak wynikow.");
    }
}

void filterByStatus(const vector<Event>& events) {
    ui::section("Filtrowanie");
    EventStatus status = chooseEventStatus();
    bool found = false;
    for (const auto& event : events) {
        if (event.status == status) {
            if (!found) {
                ui::section("Wydarzenia o wybranym statusie");
            }
            printEventRow(event);
            found = true;
        }
    }
    if (!found) {
        ui::empty("Brak wydarzen o wybranym statusie.");
    }
}

void manageParticipants(Event& event) {
    while (true) {
        ui::clear();
        ui::title("Uczestnicy - " + event.name);
        ui::menuItem(1, "Lista uczestnikow");
        ui::menuItem(2, "Dodaj uczestnika");
        ui::menuItem(3, "Zmien status platnosci");
        ui::menuItem(4, "Usun uczestnika");
        ui::menuExit("Powrot");
        int choice = readInt("Wybor: ", 0, 4);

        if (choice == 0) return;
        if (choice == 1) {
            if (event.participants.empty()) {
                ui::empty("Brak uczestnikow.");
            }
            cout << ui::Muted << "   ID  Imie i nazwisko              Email                         Platnosc\n"
                 << "  ------------------------------------------------------------------------------" << ui::Reset << '\n';
            for (const auto& p : event.participants) {
                string fullName = p.firstName + " " + p.lastName;
                cout << " " << setw(4) << right << p.id << "  "
                     << setw(28) << left << fullName.substr(0, 28)
                     << setw(30) << p.email.substr(0, 30)
                     << (p.paid ? ui::Green + string("oplacone") + ui::Reset : ui::Yellow + string("nieoplacone") + ui::Reset)
                     << right << '\n';
            }
            ui::pause();
        } else if (choice == 2) {
            Participant participant;
            participant.id = nextNestedId(event.participants);
            participant.firstName = readRequiredLine("Imie: ");
            participant.lastName = readRequiredLine("Nazwisko: ");
            participant.email = readRequiredLine("Email: ");
            participant.paid = readYesNo("Czy oplacil?");
            event.participants.push_back(participant);
            ui::success("Dodano uczestnika.");
            ui::pause();
        } else if (choice == 3) {
            int id = readInt("ID uczestnika: ", 1, numeric_limits<int>::max());
            auto it = find_if(event.participants.begin(), event.participants.end(), [id](const Participant& p) {
                return p.id == id;
            });
            if (it == event.participants.end()) {
                ui::error("Nie znaleziono uczestnika.");
            } else {
                it->paid = !it->paid;
                ui::success("Zmieniono status platnosci.");
            }
            ui::pause();
        } else if (choice == 4) {
            int id = readInt("ID uczestnika: ", 1, numeric_limits<int>::max());
            auto oldSize = event.participants.size();
            event.participants.erase(remove_if(event.participants.begin(), event.participants.end(),
                [id](const Participant& p) { return p.id == id; }), event.participants.end());
            if (event.participants.size() == oldSize) {
                ui::error("Nie znaleziono uczestnika.");
            } else {
                ui::success("Usunieto uczestnika.");
            }
            ui::pause();
        }
    }
}

void manageTasks(Event& event) {
    while (true) {
        ui::clear();
        ui::title("Zadania - " + event.name);
        ui::menuItem(1, "Lista zadan");
        ui::menuItem(2, "Dodaj zadanie");
        ui::menuItem(3, "Zmien status");
        ui::menuItem(4, "Usun zadanie");
        ui::menuExit("Powrot");
        int choice = readInt("Wybor: ", 0, 4);

        if (choice == 0) return;
        if (choice == 1) {
            if (event.tasks.empty()) {
                ui::empty("Brak zadan.");
            }
            cout << ui::Muted << "   ID  Tytul                         Osoba              Termin       Status\n"
                 << "  ------------------------------------------------------------------------------" << ui::Reset << '\n';
            for (const auto& task : event.tasks) {
                cout << " " << setw(4) << right << task.id << "  "
                     << setw(30) << left << task.title.substr(0, 30)
                     << setw(18) << task.owner.substr(0, 18)
                     << setw(13) << task.deadline.substr(0, 13)
                     << statusToString(task.status) << right << '\n';
            }
            ui::pause();
        } else if (choice == 2) {
            Task task;
            task.id = nextNestedId(event.tasks);
            task.title = readRequiredLine("Tytul: ");
            task.owner = readRequiredLine("Odpowiedzialny: ");
            task.deadline = readRequiredLine("Termin: ");
            task.status = chooseTaskStatus();
            event.tasks.push_back(task);
            ui::success("Dodano zadanie.");
            ui::pause();
        } else if (choice == 3) {
            int id = readInt("ID zadania: ", 1, numeric_limits<int>::max());
            auto it = find_if(event.tasks.begin(), event.tasks.end(), [id](const Task& task) {
                return task.id == id;
            });
            if (it == event.tasks.end()) {
                ui::error("Nie znaleziono zadania.");
            } else {
                it->status = chooseTaskStatus();
                ui::success("Zmieniono status zadania.");
            }
            ui::pause();
        } else if (choice == 4) {
            int id = readInt("ID zadania: ", 1, numeric_limits<int>::max());
            auto oldSize = event.tasks.size();
            event.tasks.erase(remove_if(event.tasks.begin(), event.tasks.end(),
                [id](const Task& task) { return task.id == id; }), event.tasks.end());
            if (event.tasks.size() == oldSize) {
                ui::error("Nie znaleziono zadania.");
            } else {
                ui::success("Usunieto zadanie.");
            }
            ui::pause();
        }
    }
}

void manageBudget(Event& event) {
    while (true) {
        ui::clear();
        ui::title("Budzet - " + event.name);
        ui::menuItem(1, "Lista pozycji");
        ui::menuItem(2, "Dodaj pozycje");
        ui::menuItem(3, "Usun pozycje");
        ui::menuExit("Powrot");
        int choice = readInt("Wybor: ", 0, 3);

        if (choice == 0) return;
        if (choice == 1) {
            double total = 0.0;
            if (event.budget.empty()) {
                ui::empty("Brak pozycji budzetowych.");
            }
            cout << ui::Muted << "   ID  Typ        Nazwa                                  Kwota\n"
                 << "  ------------------------------------------------------------------" << ui::Reset << '\n';
            for (const auto& item : event.budget) {
                double signedAmount = item.income ? item.amount : -item.amount;
                total += signedAmount;
                cout << " " << setw(4) << right << item.id << "  "
                     << setw(10) << left << (item.income ? "Przychod" : "Koszt")
                     << setw(38) << item.name.substr(0, 38)
                     << right << fixed << setprecision(2) << item.amount << " PLN\n";
            }
            ui::line(66);
            cout << ui::Bold << " Bilans: " << fixed << setprecision(2) << total << " PLN" << ui::Reset << '\n';
            ui::pause();
        } else if (choice == 2) {
            BudgetItem item;
            item.id = nextNestedId(event.budget);
            item.name = readRequiredLine("Nazwa pozycji: ");
            item.amount = readPositiveDouble("Kwota: ");
            item.income = readYesNo("Czy to przychod?");
            event.budget.push_back(item);
            ui::success("Dodano pozycje budzetowa.");
            ui::pause();
        } else if (choice == 3) {
            int id = readInt("ID pozycji: ", 1, numeric_limits<int>::max());
            auto oldSize = event.budget.size();
            event.budget.erase(remove_if(event.budget.begin(), event.budget.end(),
                [id](const BudgetItem& item) { return item.id == id; }), event.budget.end());
            if (event.budget.size() == oldSize) {
                ui::error("Nie znaleziono pozycji budzetowej.");
            } else {
                ui::success("Usunieto pozycje budzetowa.");
            }
            ui::pause();
        }
    }
}

void eventWorkspace(vector<Event>& events) {
    int id = chooseEventId(events);
    Event* event = findEvent(events, id);
    if (event == nullptr) {
        ui::error("Nie znaleziono wydarzenia.");
        return;
    }

    while (true) {
        ui::clear();
        ui::title("Panel wydarzenia");
        cout << ui::Bold << event->name << ui::Reset << '\n';
        cout << event->date << " | " << event->location << " | " << statusToString(event->status) << "\n\n";
        ui::menuItem(1, "Uczestnicy");
        ui::menuItem(2, "Zadania");
        ui::menuItem(3, "Budzet");
        ui::menuExit("Powrot");
        int choice = readInt("Wybor: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) manageParticipants(*event);
        if (choice == 2) manageTasks(*event);
        if (choice == 3) manageBudget(*event);
    }
}

void printReport(const vector<Event>& events) {
    int id = chooseEventId(events);
    const Event* event = findEvent(events, id);
    if (event == nullptr) {
        ui::error("Nie znaleziono wydarzenia.");
        return;
    }

    int paid = count_if(event->participants.begin(), event->participants.end(), [](const Participant& p) {
        return p.paid;
    });
    int doneTasks = count_if(event->tasks.begin(), event->tasks.end(), [](const Task& task) {
        return task.status == TaskStatus::Done;
    });
    double balance = 0.0;
    for (const auto& item : event->budget) {
        balance += item.income ? item.amount : -item.amount;
    }

    ui::title("Raport wydarzenia");
    cout << ui::Bold << setw(18) << left << "Nazwa:" << ui::Reset << event->name << '\n';
    cout << ui::Bold << setw(18) << left << "Data:" << ui::Reset << event->date << '\n';
    cout << ui::Bold << setw(18) << left << "Miejsce:" << ui::Reset << event->location << '\n';
    cout << ui::Bold << setw(18) << left << "Status:" << ui::Reset << statusToString(event->status) << '\n';
    cout << ui::Bold << setw(18) << left << "Opis:" << ui::Reset << event->description << '\n';
    ui::line();
    cout << setw(18) << left << "Uczestnicy:" << event->participants.size()
         << " (oplaceni: " << paid << ")\n";
    cout << setw(18) << left << "Zadania:" << event->tasks.size()
         << " (zakonczone: " << doneTasks << ")\n";
    cout << setw(18) << left << "Bilans:" << fixed << setprecision(2) << balance << " PLN\n";
}

void dashboard(const vector<Event>& events) {
    int planned = 0;
    int inProgress = 0;
    int done = 0;
    int cancelled = 0;
    int participants = 0;
    double globalBalance = 0.0;

    for (const auto& event : events) {
        planned += event.status == EventStatus::Planned;
        inProgress += event.status == EventStatus::InProgress;
        done += event.status == EventStatus::Done;
        cancelled += event.status == EventStatus::Cancelled;
        participants += static_cast<int>(event.participants.size());
        for (const auto& item : event.budget) {
            globalBalance += item.income ? item.amount : -item.amount;
        }
    }

    ui::title("Podsumowanie organizacji");
    cout << "  Wydarzenia lacznie:   " << ui::Bold << events.size() << ui::Reset << '\n';
    cout << "  Uczestnicy lacznie:   " << ui::Bold << participants << ui::Reset << '\n';
    cout << "  Globalny bilans:      " << ui::Bold << fixed << setprecision(2) << globalBalance << " PLN" << ui::Reset << "\n\n";

    cout << ui::Muted << "  Statusy wydarzen\n" << ui::Reset;
    cout << "  Planowane:  " << setw(4) << planned << "   W trakcie: " << setw(4) << inProgress << '\n';
    cout << "  Zakonczone: " << setw(4) << done << "   Anulowane: " << setw(4) << cancelled << '\n';
}

void resetEvents(vector<Event>& events, const fs::path& dataFile) {
    ui::title("Reset listy wydarzen");
    ui::warning("Ta operacja usunie wszystkie wydarzenia z listy i nadpisze plik wydarzenia.md.");

    if (!readYesNo("Czy na pewno chcesz zresetowac liste wydarzen?")) {
        ui::empty("Reset anulowany.");
        return;
    }

    events.clear();
    saveEvents(events, dataFile);
    ui::success("Lista wydarzen zostala zresetowana.");
}

int main(int argc, char* argv[]) {
    fs::path dataFile = databasePath(argc > 0 ? argv[0] : DatabaseFileName);
    fs::path legacyFile = dataFile.parent_path() / "events.db";
    vector<Event> events = loadEvents(dataFile);

    if (!fs::exists(dataFile)) {
        if (fs::exists(legacyFile)) {
            events = loadLegacyEvents(legacyFile);
        }
        saveEvents(events, dataFile);
    }

    while (true) {
        ui::clear();
        ui::title("System zarzadzania wydarzeniami");
        cout << ui::Muted << "Dane: " << dataFile.string() << " | Wydarzenia: " << events.size() << ui::Reset << "\n\n";
        ui::menuItem(1, "Podsumowanie organizacji");
        ui::menuItem(2, "Lista wydarzen");
        ui::menuItem(3, "Dodaj wydarzenie");
        ui::menuItem(4, "Edytuj wydarzenie");
        ui::menuItem(5, "Usun wydarzenie");
        ui::menuItem(6, "Szukaj wydarzen");
        ui::menuItem(7, "Filtruj po statusie");
        ui::menuItem(8, "Panel wydarzenia");
        ui::menuItem(9, "Raport wydarzenia");
        ui::menuItem(10, "Reset listy wydarzen");
        ui::menuExit("Zapisz i wyjdz");
        cout << '\n';

        int choice = readInt("Wybor: ", 0, 10);
        ui::clear();

        if (choice == 0) {
            saveEvents(events, dataFile);
            ui::success("Zapisano dane. Do widzenia.");
            return 0;
        }

        switch (choice) {
            case 1: dashboard(events); break;
            case 2: listEvents(events); break;
            case 3: addEvent(events); break;
            case 4: editEvent(events); break;
            case 5: deleteEvent(events); break;
            case 6: searchEvents(events); break;
            case 7: filterByStatus(events); break;
            case 8: eventWorkspace(events); break;
            case 9: printReport(events); break;
            case 10: resetEvents(events, dataFile); break;
            default: ui::warning("Nieznana opcja."); break;
        }

        saveEvents(events, dataFile);
        ui::pause();
    }
}
