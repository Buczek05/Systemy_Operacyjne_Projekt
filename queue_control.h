#pragma once
#include "utils.cpp"
#include <thread>
#include <chrono>
#include <ctime>

#define AVAILABLE_PLACES 3
#define FAN_LIMIT 50000

class Control {
public:
    Team team;
    int available_place;
    Control() {team=none; available_place=AVAILABLE_PLACES;};
    void check_fan(pid_t);
    void check_fan_with_children(pid_t, int);
};

Control controls[3];
pid_t first_in_queue = 0;
pid_t last_in_queue = 0;
bool control_stop = false;
