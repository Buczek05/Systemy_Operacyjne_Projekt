# Raport

# Założenia projektowe

Na stadionie piłkarskim rozegrany ma zostać mecz finałowy Ligi Mistrzów. Z uwagi na rangę imprezy ustalono rygorystyczne zasady bezpieczeństwa.

### Zasady bezpieczeństwa

1. Na stadionie może przebywać maksymalnie K kibiców. Wyjątki:
    1. VIP może zawsze wejść (oczywiście zajmuje miejsce na stadionie).
    2. Rodzic z dzieckiem może wejść nawet jeśli pozostało jedno miejsce
2. Wejście na stadion możliwe będzie tylko po przejściu drobiazgowej kontroli, mającej zapobiec wnoszeniu przedmiotów niebezpiecznych.
3. Kontrola przy wejściu jest przeprowadzana równolegle na 3 stanowiskach, na każdym z nich mogą znajdować się równocześnie maksymalnie 3 osoby.
    1. Jeśli kontrolowana jest więcej niż 1 osoba równocześnie na stanowisku, to należy zagwarantować by byli to kibice tej samej drużyny.
    2. W przypadku rodzica z dzieckiem to rodzic chce dopilnować kontroli aby nie zostało złamane żadne prawo - więc jeżeli jest kontrolowany rodzic z dzieckiem to zajmują całe stanowisko
4. Kibic oczekujący na kontrolę może przepuścić w kolejce maksymalnie 5 innych kibiców. Dłuższe czekanie wywołuje jego frustrację i agresywne zachowanie, którego należy unikać za wszelką cenę.
5. Istniej niewielka liczba kibiców VIP (mniejsza niż 0,5% * K), którzy wchodzą (i wychodzą) na stadion osobnym wejściem i nie przechodzą kontroli bezpieczeństwa.
6. Dzieci poniżej 15 roku życia wchodzą na stadion pod opieką osoby dorosłej.

Po wydaniu przez kierownika polecenia (SIGUSR1) pracownik techniczny wstrzymuje wpuszczanie kibiców.

Po wydaniu przez kierownika polecenia (SIGUSR1) pracownik techniczny wznawia wpuszczanie kibiców.

Po wydaniu przez kierownika polecenia (SIGUSR2) wszyscy kibice opuszczają stadion – w momencie gdy wszyscy kibice opuścili stadion, pracownik techniczny wysyła informację do kierownika.

# Mechanizmy komunikacji

## Kolejka komunikatów

Dla całej komunikacji bezpośredniej (proces → proces) wykorzystałem jedną kolejkę komunikatów z następującą strukturą danych

```cpp
struct FIFOMessage {
    long mtype; - odbiorca wiadomości
    pid_t sender; - nadawca wiadomości
    FIFOAction action; - typ wiadomości
    char info[100]; - dodatkowe informacje w formie tekstu
};
```

Do używania powyższej funkcjonalności przygotowałem następujące funkcje

```cpp
void create_message_queue(); - tworzenie kolejki
void clear_queue(); - czyszczenie kolejki
// Wysyłanie wiadomości z pomocą osobnego wątku
void send_message(long, FIFOAction); - bez dodatkowej zawartości
void send_message(long, FIFOAction, int); - z dodatkową zawartością typu int
void send_message(long, FIFOAction, const std::string&); - z dodatkową zawartością typu str
// Wysyłanie wiadomości
void m_send_message(long, FIFOAction, const std::string&);
FIFOMessage receive_message(long mtype); - odbierz wiadomość (paramert - odbiorca wiadomości)
void delete_message_queue(); - usuwanie kolejki
```

Aby nie przekazywać do nowych procesów pid procesu kolejki dodałem ENUM

```cpp
enum FIFOSpecialRecipient {
    CONTROL = 1,
    STADIUM = 2,
    VISUALIZATION = 3,
};
```

## Pamięć współdzielona

Dla mechanizmu “ewakuacji” wykorzystałem pamięć współdzieloną która trzyma wartość typu `int` i każdy kibic sprawdza tą wartość co określony czas (1 sekunda) - jest to symulacja wyjącej syreny na stadionie

Wszystkie funkcje kolejki komunikatów oraz pamięci współdzielonej znajdują się w pliku utils.cpp

[https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/utils.cpp](https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/utils.cpp)

## Sygnały

Zgodnie z opisem zadania wykorzystałem sygnały do zatrzymywania / wznawiania wpuszczania kibiców na stadion oraz do “ewakuacji”

# Wizualizacja

Przygotowałem prostą wizualizacje z pomocą języka python oraz biblioteki matplotlib która na żywo prezentuje zachowanie każdego kibica

Film `przedstawienie_[wizualizacji.mov](http://wizualizacji.mov)` dostępny w plikach źródłowych

# Testy

### Testy automatyczne

Przygotowane zostały testy większości funkcjonalności kibica, znajdują się w folderze test

### Testy manualne

1. Testy sygnałów
2. Uruchomienie symulacji i analiza logów konkretnej funkcjonalności
3. Analiza na wizualizacji
4. Osiąganie limitów