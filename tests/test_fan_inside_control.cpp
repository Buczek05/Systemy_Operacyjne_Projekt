#include "../fan_inside_control.cpp"
#include "utils.cpp"
#include <sstream>

void check_fan_count(FansInsideControl &fan_inside_control, int expected_count) {
    if (fan_inside_control.get_inside_fans_count() != expected_count) {
        throw TestException("Expected fan count: " + std::to_string(expected_count) + ", but got: " + std::to_string(fan_inside_control.get_inside_fans_count()));
    }
}

bool test_fan_inside_control_add_fan() {
    FansInsideControl fan_inside_control;
    for (int i = 0; i < 1000; i++) {
        fan_inside_control.add_fan_inside(i+3312);
        check_fan_count(fan_inside_control, i+1);
    }
    return true;
}

bool test_fan_inside_control_remove_fan() {
    FansInsideControl fan_inside_control;
    for (int i = 0; i < 1000; i++) {
        fan_inside_control.add_fan_inside(i+3312);
    }
    for (int i = 0; i < 1000; i++) {
        fan_inside_control.remove_fan_inside(i+3312);
        check_fan_count(fan_inside_control, 1000-i-1);
    }
    return true;
}

bool test_print_fans_inside() {
    FansInsideControl fan_inside_control;
    for (int i = 0; i < 10; i++) {
        fan_inside_control.add_fan_inside(i+3312);
        if (i % 2) {
            fan_inside_control.remove_fan_inside(i+3312);
        }
    }
    std::ostringstream output_stream;
    fan_inside_control.print_fans_inside(output_stream);
    std::string output = output_stream.str();
    std::string expected_output = "Fans inside: \nPID = 3312\nPID = 3314\nPID = 3316\nPID = 3318\nPID = 3320\n";
    return output == expected_output;
}