#pragma once
#include "utils.cpp"
#include "fan_inside_control.cpp"

#define AVAILABLE_PLACES 3
#define FAN_LIMIT 200

extern Logger logger;

class Control {
public:
    Team team;
    int available_place;
    int control_number;
    Control() {team=none; available_place=AVAILABLE_PLACES;};
    void check_fan(pid_t);
    void check_fan_with_children(pid_t, int);
};
Control controls[3];
bool control_stop = false;
pid_t first_in_queue = 0;
pid_t last_in_queue = 0;

void listen_for_message_control(Control *control);
void process_message_control(Control *control, FIFOMessage message);
void process_ready_to_control(Control *control, FIFOMessage message);
void process_ready_to_control_with_children(Control *control, FIFOMessage message);
void process_fan_nervous_abour_waiting(Control *control, FIFOMessage message);
void process_no_other_in_queue(Control *control, FIFOMessage message);

bool is_limit_reached();
void control();