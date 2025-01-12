#include "fan_inside_control.h"

FansInsideControl::FansInsideControl() {
    fan_inside_count = 0;
    fan_inside_limit = 50;
    fans_inside = nullptr;
    fans_inside = (pid_t*)malloc(fan_inside_limit * sizeof(pid_t));
    if (fans_inside == NULL) {
        std::cerr << "Error: Memory allocation failed" << std::endl;
        return;
    }
}

FansInsideControl::~FansInsideControl() {
    if (fans_inside != nullptr) {
        free(fans_inside);
    }
}

void FansInsideControl::add_new_memory_if_needed() {
    if (fan_inside_next_index == fan_inside_limit - 1) {
        fan_inside_limit += (fan_inside_limit * 0.3 > 50) ? static_cast<int>(fan_inside_limit * 0.3) : 50;
        pid_t* temp = (pid_t*)realloc(fans_inside, fan_inside_limit * sizeof(pid_t));
        if (temp == NULL) {
            std::cerr << "Error: Memory reallocation failed" << std::endl;
            return;
        }
        fans_inside = temp;
    }
}

void FansInsideControl::add_fan_inside(pid_t fan_pid, int count) {
    fan_inside_count += count;
    add_new_memory_if_needed();
    fans_inside[fan_inside_next_index - 1] = fan_pid;
    fan_inside_next_index++;
}

void FansInsideControl::remove_fan_inside(pid_t fan_pid) {
    for (int i = 0; i < fan_inside_next_index; i++) {
        if (fans_inside[i] == fan_pid) {
            fans_inside[i] = 0;
            std::cout << "Fan (PID = " << fan_pid << ") left the stadium" << std::endl;
            fan_inside_count--;
            return;
        }
    }
    std::cerr << "Fan (PID = " << fan_pid << ") not found inside" << std::endl;
}

void FansInsideControl::print_fans_inside(std::ostream &output_stream) const {
    output_stream << "Fans inside: " << std::endl;
    for (int i = 0; i < fan_inside_next_index; i++) {
        if (fans_inside[i]) {
            output_stream << "PID = " << fans_inside[i] << std::endl;
        }
    }
}