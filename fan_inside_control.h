#pragma once
#include <iostream>
#include <unistd.h>

class FansInsideControl {
private:
    int fan_inside_count;
    int fan_inside_next_index;
    int fan_inside_limit;
    pid_t *fans_inside;
    void add_new_memory_if_needed();
public:
    FansInsideControl();
    ~FansInsideControl();
    void add_fan_inside(pid_t fan_pid, int count = 1);
    void remove_fan_inside(pid_t fan_pid);
    int get_inside_fans_count() const { return fan_inside_count; }
    void print_fans_inside(std::ostream &output_stream = std::cerr) const;
};

FansInsideControl fan_inside_control;
