#include <chrono>
#include "utils.cpp"
#include "control.cpp"

#pragma once

void listen_for_messages_queue();
void set_queued_process_pid(pid_t to, int info);
void process_join_to_queue(FIFOMessage message);

void queue();