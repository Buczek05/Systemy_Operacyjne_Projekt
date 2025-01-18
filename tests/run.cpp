#include <stdio.h>
#include <time.h>
#include <iostream>
#include "utils.cpp"
#include "test_fan_inside_control.cpp"
#include "test_manager.cpp"
#include "test_fan.cpp"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"


struct Time {
    int hours, minutes, seconds;
    long nanoseconds;
};

struct Time get_time(long time_taken) {
    double seconds_total = (double) time_taken / 1e9;
    struct Time time;
    time.hours = (int) (seconds_total / 3600);
    seconds_total -= time.hours * 3600;
    time.minutes = (int) (seconds_total / 60);
    seconds_total -= time.minutes * 60;
    time.seconds = (int) seconds_total;
    seconds_total -= time.seconds;
    time.nanoseconds = (int) (seconds_total * 1e9);
    return time;
}

char *time_to_string(struct Time time) {
    char *time_str = (char *) malloc(30 * sizeof(char));
    if (time_str != nullptr) {
        snprintf(time_str, 30, "%02d:%02d:%02d.%09ld", time.hours, time.minutes, time.seconds, time.nanoseconds);
    }
    return time_str;
}

long diff(struct timespec start, struct timespec end) {
    long diff = (end.tv_sec - start.tv_sec) * (long) 1e9;
    diff += end.tv_nsec - start.tv_nsec;
    return diff;
}

void run_int_func_with_str(bool (*func_to_run)(), const char *test_name) {
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    bool result;
    try {
        result = func_to_run();
    } catch (const TestException &e) {
        result = false;
        std::cout << RED << "TestException caught in <" << test_name << ">: " << e.what() << RESET << std::endl;
    } catch (const std::exception &e) {
        result = false;
        std::cout << RED << "Exception caught in <" << test_name << ">: " << e.what() << RESET << std::endl;
    } catch (...) {
        result = false;
        std::cout << RED << "Unknown error occurred in <" << test_name << ">" << RESET << std::endl;
    }
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    struct Time time = get_time(diff(t_start, t_end));

    if (result) {
        std::cout << GREEN << "Test <" << test_name << "> PASSED; Time taken: " << time_to_string(time) << RESET << std::endl;
    } else {
        std::cout << RED << "Test <" << test_name << "> FAILED" << RESET << std::endl;
    }
}

int main() {
    testing = true;
    create_message_queue();
    create_evacuation_shared_memory();
    clear_queue();
    run_int_func_with_str(test_fan_inside_control_add_fan, "Test FanInsideControl add_fan");
    run_int_func_with_str(test_fan_inside_control_remove_fan, "Test FanInsideControl remove_fan");
    run_int_func_with_str(test_print_fans_inside, "Test FanInsideControl print_fans_inside");
    run_int_func_with_str(test_create_technic, "Test create_technic");
    run_int_func_with_str(test_create_fan, "Test create_fan");
    run_int_func_with_str(test_listen_for_messages, "Test fan listen messages");
    run_int_func_with_str(test_process_message_invite_to_control_1, "Test process message invite to control 1");
    run_int_func_with_str(test_process_message_invite_to_control_2, "Test process message invite to control 2");
    run_int_func_with_str(test_process_message_invite_to_control_3, "Test process message invite to control 3");
    run_int_func_with_str(test_process_message_invite_to_control_4, "Test process message invite to control 4");
    run_int_func_with_str(test_process_message_invite_to_control_5, "Test process message invite to control 5");
    run_int_func_with_str(test_process_message_set_queued_process_pid, "Test process message set queued process pid");
    run_int_func_with_str(test_process_message_enjoy_the_game, "Test process message enjoy the game");
    run_int_func_with_str(test_join_queue, "Test join queue");
    run_int_func_with_str(test_generate_children, "Test generate children");
    run_int_func_with_str(test_setup_random_fan_data, "Test setup random fan data");
    run_int_func_with_str(test_is_outside, "Test is outside");
    run_int_func_with_str(test_checking_evacuation, "Test checking evacuation");
    run_int_func_with_str(test_move_to, "Test move to");
    run_int_func_with_str(test_change_location, "Test change location");

    return 0;
}
