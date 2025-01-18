#include "../manager.cpp"
#include "utils.cpp"

#pragma once

bool check_process_exists(const char* process_name) {
  char command[256];
  snprintf(command, sizeof(command), "pgrep %s > /dev/null", process_name);
  int result = system(command);
  return result == 0;
}

void kill_all_processes(const char* process_name) {
    char command[256];
    snprintf(command, sizeof(command), "pkill %s", process_name);
    int result = system(command);
}

bool test_create_technic(){
  kill_all_processes("queue_control");
  bool process_exists_before = check_process_exists("queue_control");
  if (process_exists_before) {
    throw TestException("Technic process already exists");
  }

  create_technic();

  bool process_exists_after = check_process_exists("queue_control");
  if (!process_exists_after) {
    throw TestException("Technic process was not created");
  }
  kill_all_processes("queue_control");
  return !process_exists_before && process_exists_after;
}

bool test_create_fan() {
  kill_all_processes("run_fan");
  bool process_exists_before = check_process_exists("run_fan");
  if (process_exists_before) {
    throw TestException("Fan process already exists");
  }

  create_fan();

  bool process_exists_after = check_process_exists("run_fan");
  if (!process_exists_after) {
    throw TestException("Fan process was not created");
  }
  kill_all_processes("run_fan");
  return !process_exists_before && process_exists_after;
}
