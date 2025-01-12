#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>

#pragma once
pid_t technic_pid = 0;
int created_fans = 0;

int main();
void create_technic();
void create_fan();

void send_stop_start_signal();
void send_evacuation_signal();
void handle_signal(int signal);
int get_new_fans_count();