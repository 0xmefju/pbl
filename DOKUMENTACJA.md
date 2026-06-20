# Dokumentacja techniczna

## System zarządzania organizacją wydarzeń

| Pole | Wartość |
| --- | --- |
| Nazwa programu | System zarządzania organizacją wydarzeń |
| Typ aplikacji | Aplikacja konsolowa |
| Język implementacji | C++ |
| Standard języka | C++17 |
| Główny plik źródłowy | `main.cpp` |
| Plik wykonywalny | `event_manager.exe` |
| Plik danych | `wydarzenia.md` |
| Wersja dokumentu | 1.0 |
| Data opracowania | 2026-06-19 |

---

## Spis treści

1. Cel i zakres programu
2. Wymagania systemowe
3. Kompilacja programu
4. Uruchomienie aplikacji
5. Struktura menu głównego
6. Opis funkcji programu
7. Obsługa uczestników
8. Obsługa zadań
9. Obsługa budżetu
10. Struktura pliku danych
11. Walidacja danych i zabezpieczenia
12. Mechanizm zapisu danych
13. Znane ograniczenia
14. Zalecenia eksploatacyjne
15. Proponowane kierunki rozwoju
16. Najczęstsze problemy
17. Podsumowanie techniczne

---

## 1. Cel i zakres programu

Program jest konsolowym systemem przeznaczonym do zarządzania organizacją wydarzeń. Umożliwia tworzenie i utrzymywanie listy wydarzeń, edycję danych, obsługę uczestników, kontrolę zadań organizacyjnych, prowadzenie budżetu oraz generowanie raportów.

Aplikacja została zaimplementowana w języku C++ jako jeden plik źródłowy:

```text
main.cpp
```

Po skompilowaniu program uruchamiany jest jako plik wykonywalny, na przykład:

```text
event_manager.exe
```

Dane programu są zapisywane automatycznie do pliku:

```text
wydarzenia.md
```

Plik `wydarzenia.md` jest tworzony w tej samej lokalizacji co uruchomiony program.

## 2. Wymagania systemowe

Do kompilacji wymagany jest kompilator C++ obsługujący standard C++17.

Zalecany kompilator dla systemu Windows:

```text
g++
```

Program nie wymaga dodatkowych bibliotek zewnętrznych.

## 3. Kompilacja programu

W folderze projektu należy uruchomić polecenie:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o event_manager.exe
```

Po poprawnej kompilacji zostanie utworzony plik:

```text
event_manager.exe
```

## 4. Uruchomienie aplikacji

Program uruchamia się poleceniem:

```powershell
.\event_manager.exe
```

Przy pierwszym uruchomieniu aplikacja tworzy plik:

```text
wydarzenia.md
```

Jeżeli obok programu istnieje wcześniejszy plik `events.db`, a plik `wydarzenia.md` jeszcze nie istnieje, program podejmie jednorazową próbę wczytania danych ze starego pliku i zapisania ich w formacie Markdown.

## 5. Struktura menu głównego

Po uruchomieniu aplikacji użytkownik otrzymuje dostęp do menu głównego:

```text
1   Podsumowanie organizacji
2   Lista wydarzeń
3   Dodaj wydarzenie
4   Edytuj wydarzenie
5   Usuń wydarzenie
6   Szukaj wydarzeń
7   Filtruj po statusie
8   Panel wydarzenia
9   Raport wydarzenia
10  Reset listy wydarzeń
0   Zapisz i wyjdź
```

Wybór opcji odbywa się przez wpisanie numeru i zatwierdzenie klawiszem Enter.

## 6. Opis funkcji programu

### 6.1. Podsumowanie organizacji

Opcja `1` prezentuje ogólne statystyki:

- liczbę wszystkich wydarzeń,
- liczbę wszystkich uczestników,
- globalny bilans budżetu,
- liczbę wydarzeń według statusu.

Statusy wydarzeń:

- Planowane,
- W trakcie,
- Zakończone,
- Anulowane.

### 6.2. Lista wydarzeń

Opcja `2` wyświetla wszystkie zapisane wydarzenia w formie tabeli.

Tabela zawiera:

- ID wydarzenia,
- nazwę,
- datę,
- miejsce,
- status.

### 6.3. Dodawanie wydarzenia

Opcja `3` umożliwia dodanie nowego wydarzenia.

Wymagane pola:

- nazwa,
- data,
- miejsce,
- status.

Pole opcjonalne:

- opis.

Data musi zostać wpisana w formacie:

```text
RRRR-MM-DD
```

Przykłady poprawnych dat:

```text
2026-08-03
2028-02-29
```

Przykłady odrzucanych wartości:

```text
abc
23
2026-02-30
2026-99-99
2026-8-3
```

### 6.4. Edycja wydarzenia

Opcja `4` pozwala zmienić dane istniejącego wydarzenia.

Najpierw należy wybrać ID wydarzenia. Podczas edycji można pozostawić pole puste, aby zachować obecną wartość.

Przy edycji daty również obowiązuje format:

```text
RRRR-MM-DD
```

### 6.5. Usuwanie wydarzenia

Opcja `5` usuwa wybrane wydarzenie.

Program wymaga potwierdzenia przed usunięciem:

```text
Usunac wydarzenie "nazwa"? (t/n)
```

Dzięki temu przypadkowe usunięcie jest ograniczone.

### 6.6. Wyszukiwanie wydarzeń

Opcja `6` wyszukuje wydarzenia po:

- nazwie,
- miejscu,
- opisie.

Pole wyszukiwania nie może być puste.

### 6.7. Filtrowanie po statusie

Opcja `7` wyświetla tylko wydarzenia o wybranym statusie.

Dostępne statusy:

```text
1  Planowane
2  W trakcie
3  Zakończone
4  Anulowane
```

### 6.8. Panel wydarzenia

Opcja `8` otwiera panel konkretnego wydarzenia.

W panelu wydarzenia dostępne są:

```text
1  Uczestnicy
2  Zadania
3  Budżet
0  Powrót
```

### 6.9. Raport wydarzenia

Opcja `9` wyświetla raport jednego wydarzenia.

Raport zawiera:

- nazwę,
- datę,
- miejsce,
- status,
- opis,
- liczbę uczestników,
- liczbę opłaconych uczestników,
- liczbę zadań,
- liczbę zakończonych zadań,
- bilans budżetu.

### 6.10. Reset listy wydarzeń

Opcja `10` usuwa wszystkie wydarzenia z programu.

Program wymaga potwierdzenia:

```text
Czy na pewno chcesz zresetowac liste wydarzen? (t/n)
```

Po potwierdzeniu:

- lista wydarzeń zostaje wyczyszczona,
- plik `wydarzenia.md` zostaje nadpisany pustą listą,
- sam plik `wydarzenia.md` nie jest usuwany.

## 7. Obsługa uczestników

Uczestnicy są zarządzani z poziomu panelu wydarzenia.

Dostępne opcje:

```text
1  Lista uczestników
2  Dodaj uczestnika
3  Zmień status płatności
4  Usuń uczestnika
0  Powrót
```

### 7.1. Dodawanie uczestnika

Wymagane pola:

- imię,
- nazwisko,
- email,
- status płatności.

Status płatności wybiera się odpowiedzią:

```text
t
n
```

Akceptowane są również odpowiedzi:

```text
tak
nie
y
yes
no
```

### 7.2. Zmiana statusu płatności

Opcja zmiany statusu płatności przełącza wartość:

- z `nieopłacone` na `opłacone`,
- z `opłacone` na `nieopłacone`.

### 7.3. Usuwanie uczestnika

Usuwanie odbywa się po ID uczestnika.

Jeżeli ID nie istnieje, program wyświetla komunikat błędu.

## 8. Obsługa zadań

Zadania są zarządzane z poziomu panelu wydarzenia.

Dostępne opcje:

```text
1  Lista zadań
2  Dodaj zadanie
3  Zmień status
4  Usuń zadanie
0  Powrót
```

### 8.1. Dodawanie zadania

Wymagane pola:

- tytuł,
- osoba odpowiedzialna,
- termin,
- status.

Status zadania:

```text
1  Do zrobienia
2  W trakcie
3  Gotowe
```

### 8.2. Zmiana statusu zadania

Status zadania można zmienić po podaniu ID zadania.

### 8.3. Usuwanie zadania

Usuwanie odbywa się po ID zadania.

Jeżeli ID nie istnieje, program wyświetla komunikat błędu.

## 9. Obsługa budżetu

Budżet jest zarządzany z poziomu panelu wydarzenia.

Dostępne opcje:

```text
1  Lista pozycji
2  Dodaj pozycję
3  Usuń pozycję
0  Powrót
```

### 9.1. Dodawanie pozycji budżetowej

Wymagane pola:

- nazwa pozycji,
- kwota,
- typ pozycji.

Kwota musi być większa od zera.

Typ pozycji:

- przychód,
- koszt.

Program oblicza bilans według zasady:

```text
bilans = suma przychodów - suma kosztów
```

### 9.2. Usuwanie pozycji budżetowej

Usuwanie odbywa się po ID pozycji budżetowej.

Jeżeli ID nie istnieje, program wyświetla komunikat błędu.

## 10. Struktura pliku danych `wydarzenia.md`

Program zapisuje dane w pliku Markdown:

```text
wydarzenia.md
```

Plik składa się z dwóch części:

1. Czytelnego raportu Markdown.
2. Technicznego bloku danych.

Przykład:

````markdown
# Wydarzenia

## Konferencja IT

- ID: 1
- Data: 2026-08-03
- Miejsce: Warszawa
- Status: Planowane

```event-manager-data
EVENT|1|Konferencja IT|2026-08-03|Warszawa|Opis|1
```
````

Blok:

```text
event-manager-data
```

jest wymagany do ponownego wczytania danych przez program.

Nie należy usuwać ani ręcznie modyfikować tego bloku, jeżeli dane mają być dalej poprawnie odczytywane.

## 11. Walidacja danych i zabezpieczenia

Program blokuje lub ogranicza następujące sytuacje:

- wybór opcji spoza zakresu menu,
- puste wymagane pola przy dodawaniu wydarzenia,
- puste wymagane pola przy dodawaniu uczestnika,
- puste wymagane pola przy dodawaniu zadania,
- puste wymagane pole przy dodawaniu pozycji budżetowej,
- puste pole wyszukiwania,
- datę w niepoprawnym formacie,
- nieistniejące daty, na przykład `2026-02-30`,
- kwoty budżetowe mniejsze lub równe zero,
- usunięcie wydarzenia bez potwierdzenia,
- reset listy wydarzeń bez potwierdzenia,
- usunięcie uczestnika, zadania lub pozycji budżetowej o nieistniejącym ID.

## 12. Mechanizm zapisu danych

Dane są zapisywane:

- po wykonaniu operacji w menu głównym,
- przy wyjściu z programu opcją `0`,
- po resecie listy wydarzeń.

Oznacza to, że większość zmian jest zapisywana automatycznie.

## 13. Znane ograniczenia

Program jest aplikacją konsolową, a nie okienkową.

Aktualne ograniczenia:

- brak logowania użytkowników,
- brak uprawnień administratora i użytkownika,
- brak eksportu do PDF,
- brak sortowania wydarzeń po dacie,
- brak sprawdzania poprawności adresu email,
- termin zadania jest zwykłym tekstem i nie ma jeszcze walidacji daty,
- ręczna edycja bloku `event-manager-data` może uszkodzić odczyt danych,
- reset usuwa wszystkie wydarzenia po potwierdzeniu i nie ma mechanizmu kosza.

## 14. Zalecenia eksploatacyjne

Przed resetem listy wydarzeń zaleca się wykonanie kopii pliku:

```text
wydarzenia.md
```

Nie należy edytować technicznego bloku danych, jeżeli nie jest to konieczne.

Jeżeli plik `wydarzenia.md` zostanie uszkodzony, program może pominąć część rekordów podczas odczytu.

## 15. Proponowane kierunki rozwoju

Możliwe kierunki rozwoju:

- walidacja adresu email,
- walidacja terminu zadania jako daty,
- sortowanie wydarzeń po dacie,
- eksport raportu do osobnego pliku Markdown,
- tworzenie kopii zapasowej przed resetem,
- dodatkowe potwierdzenia przy usuwaniu uczestników, zadań i pozycji budżetowych,
- wyszukiwanie uczestników,
- archiwum wydarzeń zakończonych,
- statystyki budżetowe dla wszystkich wydarzeń,
- wersja okienkowa w Qt albo wxWidgets.

## 16. Najczęstsze problemy

### 16.1. Program nie uruchamia się z pliku `main.cpp`

Plik `main.cpp` jest kodem źródłowym. Najpierw należy skompilować go do pliku `.exe`.

### 16.2. Komenda `g++` nie działa

Oznacza to, że kompilator nie jest zainstalowany albo nie został dodany do zmiennej `PATH`.

### 16.3. Plik `wydarzenia.md` się nie tworzy

Należy sprawdzić, czy program ma prawo zapisu w folderze, w którym znajduje się `event_manager.exe`.

### 16.4. Program nie widzi ręcznie wpisanych danych

Program odczytuje dane z bloku:

```text
event-manager-data
```

Zwykły tekst Markdown jest raportem dla człowieka, ale nie jest źródłem danych dla programu.

## 17. Podsumowanie techniczne

Najważniejsze elementy kodu:

- `Event` - struktura wydarzenia,
- `Participant` - struktura uczestnika,
- `Task` - struktura zadania,
- `BudgetItem` - struktura pozycji budżetowej,
- `saveEvents` - zapis danych do Markdown,
- `loadEvents` - odczyt danych z Markdown,
- `addEvent` - dodawanie wydarzenia,
- `editEvent` - edycja wydarzenia,
- `deleteEvent` - usuwanie wydarzenia,
- `eventWorkspace` - panel konkretnego wydarzenia,
- `resetEvents` - reset listy wydarzeń,
- `readDate` - walidacja daty wydarzenia.
