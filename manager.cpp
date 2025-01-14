#include "manager.h"

void create_technic(){
    technic_pid = fork();
    if (technic_pid < 0) {
        std::cerr << "Fork failed for technic" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (technic_pid == 0) {
        execlp("./queue_control", "queue_control", nullptr);
        exit(EXIT_FAILURE);
    }
}

void create_fan(){
    std::this_thread::sleep_for(std::chrono:: milliseconds(100));
    created_fans++;
    pid_t fan_pid = fork();
    if (fan_pid < 0) {
        std::cerr << "Fork failed for fan" << created_fans << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fan_pid == 0) {
        execlp("./fan", "fan", nullptr);
        exit(EXIT_FAILURE);
    }
}

void send_stop_start_signal(){
    kill(technic_pid, SIGUSR1);
}

void send_evacuation_signal(){
    kill(technic_pid, SIGUSR2);
}

void handle_signal(int signal){
  if (signal == SIGUSR1) send_stop_start_signal();
  else if (signal == SIGUSR2) send_evacuation_signal();
  else std::cerr << "Unknown signal" << std::endl;
}

int get_new_fans_count(){
    int new_fans_count = std::rand() % ((10000 - created_fans) / 600) + 1;
    if (100000 - created_fans < 0) new_fans_count = std::rand() % 5;
    std::cout << "New fans count: " << new_fans_count << std::endl;
    return new_fans_count;
}
