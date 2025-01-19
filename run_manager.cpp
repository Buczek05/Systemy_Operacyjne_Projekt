#include "manager.cpp"

int main() {
    logger.~Logger();
    new(&logger) Logger("logs/manager");
    logger << "Manager started";
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);
    create_technic();

    while (true) {
        sleep(1);
        int new_fans_count = get_new_fans_count();
        for (int i = 0; i < new_fans_count; i++) create_fan();
        if (rand() % 1000 == 0) send_stop_start_signal();
        if (rand() % 100000 == 0) send_evacuation_signal();
    }
    return 0;
}
