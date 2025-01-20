#include "fan_inside_control.h"

#include "logs.h"

extern Logger logger;

FansInsideControl::FansInsideControl() {
    fan_inside_count = 0;
    fan_inside_limit = 50;
    fans_inside = nullptr;
    fans_inside = (pid_t*)malloc(fan_inside_limit * sizeof(pid_t));
    if (fans_inside == NULL) {
        logger << "Error: Memory allocation failed";
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
            logger << "Error: Memory reallocation failed";
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
    std::ostringstream logStream;
    for (int i = 0; i < fan_inside_next_index; i++) {
        if (fans_inside[i] == fan_pid) {
            fans_inside[i] = 0;
            logStream.str("");
            logStream << "Fan (PID = " << fan_pid << ") left the stadium";
            logger << logStream.str();
            fan_inside_count--;
            return;
        }
    }
    logStream.str("");
    logStream << "Fan (PID = " << fan_pid << ") not found inside";
    logger << logStream.str();
}

void FansInsideControl::print_fans_inside(std::ostream &output_stream) const {
    for (int i = 0; i < fan_inside_next_index; i++) {
        if (fans_inside[i]) {
            output_stream << "PID = " << fans_inside[i] << std::endl;
        }
    }
}