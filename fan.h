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

int other_fan_let_count = 0;
Team team;
bool VIP;
int age;
FanPlace place;
pid_t queued_process_pid = 0;

int children_count = 0;
Child *children = NULL;