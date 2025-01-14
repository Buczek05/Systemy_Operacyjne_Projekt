#include <stdio.h>
#include <time.h>
#include <iostream>
#include "utils.cpp"
#include "test_fan_inside_control.cpp"
#include "test_manager.cpp"

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
    run_int_func_with_str(test_fan_inside_control_add_fan, "Test FanInsideControl add_fan");
    run_int_func_with_str(test_fan_inside_control_remove_fan, "Test FanInsideControl remove_fan");
    run_int_func_with_str(test_print_fans_inside, "Test FanInsideControl print_fans_inside");
    run_int_func_with_str(test_create_technic, "Test create_technic");
    run_int_func_with_str(test_create_fan, "Test create_fan");
    return 0;
}
