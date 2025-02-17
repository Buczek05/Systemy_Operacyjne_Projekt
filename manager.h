#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "utils.cpp"
#include "logs.cpp"
#include "technic.cpp"
#include "fan.cpp"

#pragma once

#define MAX_FAN_PROCESSES 1000

pid_t technic_pid = 0;
int created_fans = 0;


void create_technic();
void create_fan();

void send_stop_start_signal();
void send_evacuation_signal();
void handle_signal(int signal);
int get_new_fans_count();