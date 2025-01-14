# pragma once
#include "utils.cpp"
#include <thread>
#include <chrono>

class Child {
public:
    int age;
    Child(int a) : age(a) {};
    Child() : age(0) {};
};
void move_to(const FanPlace moving_place, const FanPlace destination);
void change_location();
int other_fan_let_count = 0;
Team team;
bool VIP;
int age;
FanPlace place = OnTheWay;
pid_t queued_process_pid = 0;

int children_count = 0;
Child *children = NULL;