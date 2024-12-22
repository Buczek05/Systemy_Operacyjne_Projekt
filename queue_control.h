#pragma once
#include "utils.cpp"
#include <thread>
#include <chrono>
#include <ctime>

class Control {
public:
    Team team;
    int available_place;
    Control() {team=none; available_place=3;};
    void check_fan(pid_t);
};

Control controls[3];
pid_t first_in_queue = 0;
pid_t last_in_queue = 0;
