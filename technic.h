#pragma once
#include "logs.cpp"
#include "control.cpp"
#include "queue.cpp"

void handle_exit_signal(int signal);

void stop_control(int sig);
void start_evacuation(int sig);
void listen_for_messages_stadium();

void technic();