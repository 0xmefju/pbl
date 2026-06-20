# System zarzadzania organizacja wydarzen

Jednoplikowa aplikacja konsolowa w C++ do zarzadzania wydarzeniami, uczestnikami, zadaniami, budzetem i raportami.

## Najwazniejsze zalozenia

- caly kod programu znajduje sie w pliku `main.cpp`
- program zapisuje dane w pliku `wydarzenia.md`
- plik `wydarzenia.md` tworzy sie automatycznie przy pierwszym uruchomieniu
- `wydarzenia.md` powstaje w tej samej lokalizacji co uruchomiony program
- plik Markdown jest czytelny dla czlowieka i zawiera blok danych potrzebny do ponownego wczytania

## Kompilacja

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o event_manager.exe
```

## Uruchomienie

```powershell
.\event_manager.exe
```

Po pierwszym uruchomieniu obok `event_manager.exe` zostanie utworzony plik:

```text
wydarzenia.md
```

Pelny opis programu znajduje sie w pliku:

```text
DOKUMENTACJA.md
```
