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

Film `przedstawienie_wizualizacji.mov` dostępny w plikach źródłowych

# Procedury

### **`technic()`**

- **Lokalizacja:** https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/technic.cpp
- **Opis:**
   - Rozpoczyna proces techniczny na stadionie.
   - Konfiguruje sygnały:
      - `SIGUSR1` – zatrzymanie/wznowienie kontroli.
      - `SIGUSR2` – rozpoczęcie ewakuacji.
      - `SIGINT` i `SIGTERM` – zakończenie procesu technicznego.
   - Tworzy kolejkę wiadomości oraz współdzieloną pamięć dla sygnalizacji ewakuacji.
   - Uruchamia w oddzielnych wątkach:
      - Kolejkę (`queue()`).
      - Nasłuchiwanie wiadomości dotyczących stadionu (`listen_for_messages_stadium()`).
      - Kontrole stadionowe (`control()`).
   - Utrzymuje działanie procesu w pętli nieskończonej, z opóźnieniem jednej sekundy.

---

### **`fan()`**

- **Lokalizacja:** https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/fan.cpp
- **Opis:**
   - Symuluje zachowanie pojedynczego kibica na stadionie.
   - Wykonuje następujące kroki:
      1. Konfiguruje dziennik logów.
      2. Generuje dane kibica, takie jak:
         - Drużyna, wiek, status VIP, liczba dzieci oraz ich wiek.
      3. Tworzy kolejkę wiadomości oraz współdzieloną pamięć dla ewakuacji.
      4. Uruchamia wątki:
         - Sprawdzanie sygnału ewakuacyjnego (`checking_evacuation`).
         - Nasłuchiwanie wiadomości dla kibica (`listen_for_messages_fan`).
      5. Dołącza do kolejki.
      6. W nieskończonej pętli monitoruje możliwość zmiany lokalizacji na stadionie (`change_location_if_want`).

---

### **`main()`**

- **Lokalizacja:** https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/run_manager.cpp
- **Opis:**
   - Jest punktem startowym aplikacji.
   - Funkcjonalność:
      1. Konfiguruje dziennik logów (`logger`).
      2. Uruchamia proces techniczny za pomocą funkcji `create_technic()`.
      3. Konfiguruje sygnały:
         - `SIGUSR1` – wywołuje funkcję `send_stop_start_signal()`.
         - `SIGUSR2` – wywołuje funkcję `send_evacuation_signal()`.
      4. W nieskończonej pętli:
         - Co sekundę generuje losową liczbę nowych kibiców (`get_new_fans_count`) i tworzy ich procesy (`create_fan()`).
         - Co pewien czas:
            - Wysyła sygnał zatrzymania/wznowienia kontroli.
            - Wysyła sygnał ewakuacji.

### **`queue()`**

- **Lokalizacja:** https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/queue.cpp
- **Opis:**
   - Symuluje działanie kolejki kibiców czekających na wejście na stadion.
   - Funkcjonalność:
      1. Nasłuchuje wiadomości skierowanych do kolejki (`listen_for_messages_queue()`).
      2. Obsługuje wiadomości:
         - Dodanie kibica do kolejki (`process_join_to_queue`).
         - Ustawienie procesu w kolejce (`set_queued_process_pid`).
      3. Zarządza relacjami między pierwszym i ostatnim kibicem w kolejce.
   - Zapewnia płynne zarządzanie kolejką przed wejściem na stadion.

---

### **`control()`**

- **Lokalizacja:** https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/control.cpp
- **Opis:**
   - Symuluje funkcjonowanie punktów kontroli na stadionie (trzy punkty).
   - Funkcjonalność:
      1. Tworzy i konfiguruje punkty kontroli (`Control`).
      2. W nieskończonej pętli:
         - Monitoruje limity kibiców na stadionie (`is_limit_reached`).
         - Sprawdza dostępność miejsc w punktach kontroli.
         - Przekazuje pierwszego kibica z kolejki do wolnego punktu kontroli (`listen_for_message_control`).
         - Obsługuje różne akcje dla kibiców, w tym:
            - Sprawdzanie kibica (`check_fan`).
            - Sprawdzanie kibica z dziećmi (`check_fan_with_children`).
      3. Generuje logi oraz wizualizacje związane z procesami kontroli.
   - Koordynuje działanie wszystkich punktów kontroli, dbając o zachowanie kolejności i płynności w obsłudze kibiców.

---

# Funkcje

### Kontrola (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/control.cpp)

1. **`listen_for_message_control(Control *control)`**
   - Odbiera wiadomości dla danej kontroli i przekazuje je do odpowiedniego przetwarzania.
2. **`process_message_control(Control *control, FIFOMessage message)`**
   - Przetwarza wiadomość na podstawie akcji:
      - `READY_TO_CONTROL`
      - `READY_TO_CONTROL_WITH_CHILDREN`
      - `FAN_NERVOUS_ABOUT_WAITING`
      - `NO_OTHER_IN_QUEUE`
3. **`check_fan(pid_t fan_pid)`**
   - Symuluje sprawdzanie pojedynczego kibica.
4. **`check_fan_with_children(pid_t fan_pid, int children_count)`**
   - Sprawdza kibica z dziećmi.
5. **`control()`**
   - Zarządza kontrolą na stadionie (3 punkty kontroli).

---

### Kibic (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/fan.cpp)

1. **`listen_for_messages_fan()`**
   - Nasłuchuje wiadomości dla kibica.
2. **`process_message(FIFOMessage message)`**
   - Obsługuje akcje:
      - `SET_QUEUED_PROCESS_PID`
      - `INVITE_TO_CONTROL`
      - `ENJOY_THE_GAME`
3. **`join_queue()`**
   - Kibic dołącza do kolejki.
4. **`generate_children()`**
   - Generuje dzieci towarzyszące kibicowi.
5. **`change_location()`**
   - Symuluje przemieszczanie się kibica po stadionie.
6. **`fan()`**
   - Główna funkcja symulująca zachowanie kibica.

---

### Zarządzanie (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/manager.cpp)

1. **`create_technic()`**
   - Tworzy proces obsługi technicznej.
2. **`create_fan()`**
   - Tworzy proces kibica.
3. **`send_stop_start_signal()`**
   - Wysyła sygnał zatrzymania/wznowienia kontroli.
4. **`send_evacuation_signal()`**
   - Wysyła sygnał ewakuacji.
5. **`get_new_fans_count()`**
   - Generuje liczbę nowych kibiców.

---

### Kolejka (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/queue.cpp)

1. **`listen_for_messages_queue()`**
   - Nasłuchuje wiadomości dla kolejki.
2. **`process_join_to_queue(FIFOMessage message)`**
   - Obsługuje dołączanie kibiców do kolejki.
3. **`queue()`**
   - Główna funkcja symulująca działanie kolejki.

---

### Techniczne (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/technic.cpp)

1. **`handle_exit_signal(int signal)`**
   - Obsługuje sygnał zakończenia procesu.
2. **`stop_control(int sig)`**
   - Zatrzymuje lub wznawia kontrolę.
3. **`start_evacuation(int sig)`**
   - Rozpoczyna proces ewakuacji.
4. **`listen_for_messages_stadium()`**
   - Nasłuchuje wiadomości związanych ze stadionem.
5. **`technic()`**
   - Główna funkcja procesu technicznego.

---

### Narzędzia (https://github.com/Buczek05/Systemy_Operacyjne_Projekt/blob/main/utils.cpp)

1. **`create_message_queue()`**
   - Tworzy kolejkę wiadomości IPC.
2. **`clear_queue()`**
   - Czyści kolejkę wiadomości.
3. **`send_message(long mtype, FIFOAction action, const std::string& info)`**
   - Wysyła wiadomość do kolejki.
4. **`receive_message(long mtype)`**
   - Odbiera wiadomość z kolejki.
5. **`create_evacuation_shared_memory()`**
   - Tworzy współdzieloną pamięć do sygnalizacji ewakuacji.
6. **`delete_evacuation_shared_memory()`**
   - Usuwa współdzieloną pamięć.
7. **`get_random_number(int from, int to)`**
   - Generuje losową liczbę w podanym zakresie.
8. **`s_sleep(int seconds)` i `ms_sleep(int ms)`**
   - Symuluje opóźnienia.

# Testy automatyczne

Przygotowane zostały testy większości funkcjonalności kibica, znajdują się w folderze test

# Testy Manualne

### 1. **Sprawdzenie, czy do jednego stanowiska może wejść ktoś poza rodzicem z dzieckiem**

- **Cel testu:**
   - Zweryfikowanie, czy system prawidłowo wymusza zasady dotyczące wejścia do stanowiska kontroli, gdzie rodzic z dzieckiem zajmuje całe jedno stanowisko
- **Szczegóły:**
   - Symulacja sytuacji, w której:
     Kibic próbuje wejść na stanowisko na którym znajduje się inny kibic z dzieckiem
   - Oczekiwane wyniki:
      - Kibic nie zostanie wpuszczony na to stanowisko
      - Kibic z dziećmi nie wpłynie na działanie pozostałych stanowisk
   - Specjalne ustawienia - BRAK
- **Rezultat**:
   - Pozytywny - kibic z dzieckiem zajmuje całe stanowisko

---

### 2. **Sprawdzenie, czy VIP zawsze może wejść na stadion – nawet gdy jest przepełniony limit**

- **Cel testu:**
   - Upewnienie się, że status VIP pozwala na pominięcie limitu miejsc na stadionie, zapewniając uprzywilejowany dostęp.
- **Szczegóły:**
   - Symulacja scenariusza:
      1. Stadion osiąga maksymalny limit kibiców (200 osób w kontrolowanej sekcji).
      2. Kibic VIP próbuje wejść na stadion.
   - Oczekiwane wyniki:
      - Kibic VIP zostaje wpuszczony, niezależnie od wypełnionego limitu.
      - System nie odnotowuje błędów, a logi potwierdzają, że VIP został obsłużony zgodnie z zasadami.
   - Specjalne ustawienia:
      - Ustawiam limit na 10 osób `FAN_LIMIT`
      - Tworze samych VIPów
- **Rezultat**:
   - Pozytywny - VIPy wchodzili bez problemu

---

### 3. **Sprawdzenie, czy dany kibic może przepuścić innych kibiców w kolejce, ale nie więcej niż 5**

- **Cel testu:**
   - Weryfikacja poprawnego działania mechanizmu umożliwiającego kibicom przepuszczanie innych w kolejce, z zachowaniem ograniczenia do 5 osób.
- **Szczegóły:**
   - Symulacja:
      1. Kibic znajdujący się w kolejce umożliwia innym kibicom przejście przed siebie.
      2. Po 5 przepuszczeniach mechanizm odmawia dalszego przepuszczania.
   - Oczekiwane wyniki:
      - Kibic może przepuścić maksymalnie 5 innych osób.
      - Po przekroczeniu limitu kibic wysyła wiadomość do kolejki `FAN_NERVOUS_ABOUT_WAITING`
   - Specjalne ustawienia - BRAK
- **Rezultat:**
   - Pozytywny - VIP przepuścił 5 osób i nikogo więcej

       ```sql
       Kontrola otrzymała wiadomość: Action: 6 (FAN_NERVOUS_ABOUT_WAITING), Sender: 69611, Info: 
       Fan (PID = 69611 ) is nervous about waiting
       Kontrola otrzymała wiadomość: Action: 6 (FAN_NERVOUS_ABOUT_WAITING), Sender: 69611, Info: 
       Fan (PID = 69611 ) is nervous about waiting
       Kontrola otrzymała wiadomość: Action: 6 (FAN_NERVOUS_ABOUT_WAITING), Sender: 69611, Info: 
       Fan (PID = 69611 ) is nervous about waiting
       Kibic o PID: 69609 został sprawdzony. w kontroli nr: 1 - został sprawdzony ostatni kibic w kontroli numer 1
       Kontrola otrzymała wiadomość: Action: 4 (READY_TO_CONTROL), Sender: 69611, Info: 2
       Sprawdzanie kibica o PID: 69611 w kontroli nr: 1
       ```


---

### 4. **Sprawdzenie, czy każdy kibic opuści stadion podczas ewakuacji i czy nikt więcej nie zostanie wpuszczony**

- **Cel testu:**
   - Upewnienie się, że w przypadku ewakuacji wszyscy kibice opuszczą stadion, a system zablokuje wpuszczanie nowych osób.
- **Szczegóły:**
   - Symulacja ewakuacji:
      1. Na stadionie znajduje się grupa kibiców, zarówno na stanowiskach kontroli, jak i na trybunach.
      2. System odbiera sygnał ewakuacji (`SIGUSR2`).
      3. Wszystkie procesy związane z wpuszczaniem kibiców zostają zatrzymane.
   - Oczekiwane wyniki:
      - Wszyscy kibice opuszczają stadion.
      - System loguje, jeśli któryś kibic pozostaje na stadionie lub jeśli wystąpią problemy z ewakuacją.
      - Nie są obsługiwane nowe wejścia na stadion w trakcie ewakuacji.
   - Specjalne ustawienia - BRAK
- **Rezultat:**
   - Pozytywny - wszyscy kibice opuścili stadion a żaden nowy nie wszedł