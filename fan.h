# pragma once
#include "utils.cpp"
#include "logs.cpp"
#include <thread>
#include <chrono>

class Child {
public:
    int age;
    Child(int a) : age(a) {};
    Child() : age(0) {};
};
int other_fan_let_count = 0;
Team team;
bool VIP;
int age;
FanPlace place = OnTheWay;
pid_t queued_process_pid = 0;

int children_count = 0;
Child *children = NULL;

std::thread *child_threads = NULL;

void listen_for_messages_fan();
void process_message(FIFOMessage message);
void process_set_queued_process_pid(FIFOMessage message);
void process_invite_to_control(FIFOMessage message);
void process_enjoy_the_game(FIFOMessage message);
void join_queue();
void generate_children();
void setup_random_fan_data();
bool is_outside();
void checking_evacuation();
void move_to(const FanPlace moving_place, const FanPlace destination);
void change_location();
void change_location_if_want();
void create_child();

void fan();