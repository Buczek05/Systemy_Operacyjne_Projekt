#include "manager.h"

void create_technic(){
    technic_pid = fork();
    if (technic_pid < 0) {
        std::ostringstream logStream;
        logStream << "Fork failed for technic";
        logger << logStream.str();
        exit(EXIT_FAILURE);
    }
    if (technic_pid == 0) {
        technic();
        exit(EXIT_FAILURE);
    }
}

void create_fan(){
    ms_sleep(100);
    created_fans++;
    pid_t fan_pid = fork();
    if (fan_pid < 0) {
        std::ostringstream logStream;
        logStream << "Fork failed for fan" << created_fans;
        logger << logStream.str();
        exit(EXIT_FAILURE);
    }
    if (fan_pid == 0) {
        fan();
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
  else logger << "Unknown signal";
}

int get_new_fans_count(){
    if (MAX_FAN_PROCESSES <= created_fans) return 0;
    int new_fans_count = std::rand() % ((10000 - created_fans) / 600) + 1;
    if (100000 - created_fans < 0) new_fans_count = std::rand() % 5;
    std::ostringstream logStream;
    logStream << "New fans count: " << new_fans_count;
    logger << logStream.str();
    return new_fans_count;
}
