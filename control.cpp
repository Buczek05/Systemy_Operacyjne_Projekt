#pragma once
#include "control.h"

void listen_for_message_control(Control *control) {
    FIFOMessage message = receive_message(CONTROL);
    std::ostringstream logStream;
    logStream << "Kontrola otrzymała wiadomość: "
              << "Action: " << message.action << ", Sender: " << message.sender
              << ", Info: " << message.info;
    logger << logStream.str();
    process_message_control(control, message);
}

void process_message_control(Control *control, FIFOMessage message) {
    switch (message.action) {
    case READY_TO_CONTROL:
        process_ready_to_control(control, message);
        break;
    case READY_TO_CONTROL_WITH_CHILDREN:
        process_ready_to_control_with_children(control, message);
        break;
    case FAN_NERVOUS_ABOUT_WAITING:
        process_fan_nervous_abour_waiting(control, message);
        break;
    case NO_OTHER_IN_QUEUE:
        process_no_other_in_queue(control, message);
        break;
    default:
        break;
    }
}

void process_ready_to_control(Control *control, FIFOMessage message) {
    control->team = (Team)std::stoi(message.info);
    control->available_place--;
    fan_inside_control.add_fan_inside(message.sender);
    std::thread worker(&Control::check_fan, control, message.sender);
    worker.detach();
}

void process_ready_to_control_with_children(Control *control, FIFOMessage message) {
    int children_count = std::stoi(message.info);
    control->available_place = 0;
    fan_inside_control.add_fan_inside(message.sender, children_count + 1);
    std::thread worker(&Control::check_fan_with_children, control, message.sender, children_count);
    worker.detach();
}

void process_fan_nervous_abour_waiting(Control *control, FIFOMessage message) {
    std::ostringstream logStream;
    logStream.str("");
    logStream << "Fan (PID = " << message.sender << " ) is nervous about waiting";
    logger << logStream.str();
}

void process_no_other_in_queue(Control *control, FIFOMessage message) {
    std::ostringstream logStream;
    logStream.str("");
    logStream << "No other in queue";
    logger << logStream.str();
}

void Control::check_fan(pid_t fan_pid) {
    std::ostringstream visualization_message;
    visualization_message << "{\"type\": \"check_fan\", \"fan_pid\": " << fan_pid
            << ", \"children_count\": " << 0 << ", \"control_number\": " << control_number << "}";
    send_to_visualization(visualization_message.str());

    std::ostringstream logStream;
    logStream << "Sprawdzanie kibica o PID: " << fan_pid << " w kontroli nr: " << control_number;
    logger << logStream.str();
    int wait_time = get_random_number(1, 10);
    s_sleep(wait_time);
    available_place++;
    logStream.str("");
    logStream << "Kibic o PID: " << fan_pid << " został sprawdzony." << " w kontroli nr: " << control_number;
    logger << logStream.str();
    send_message(fan_pid, ENJOY_THE_GAME);
}

void Control::check_fan_with_children(pid_t fan_pid, int children_count) {
    std::ostringstream visualization_message;
    visualization_message << "{\"type\": \"check_fan\", \"fan_pid\": " << fan_pid
            << ", \"children_count\": " << children_count << ", \"control_number\": " << control_number << "}";
    send_to_visualization(visualization_message.str());

    std::ostringstream logStream;
    logStream << "Sprawdzanie kibica o PID: " << fan_pid << " i " << children_count << " dzieci" << " w kontroli nr: " << control_number;
    logger << logStream.str();
    std::srand(std::time(nullptr));
    for (int i = 0; i < children_count; i++) {
        int wait_time = get_random_number(1, 10);
        s_sleep(wait_time);
    }
    available_place = AVAILABLE_PLACES;
    logStream.str("");
    logStream << "Kibic o PID: " << fan_pid << " i " << children_count << " dzieci został sprawdzony." << " w kontroli nr: " << control_number;
    logger << logStream.str();
    send_message(fan_pid, ENJOY_THE_GAME);
}

bool is_limit_reached() {
    if (fan_inside_control.get_inside_fans_count() >= FAN_LIMIT){
        logger << "Limit reached";
        return true;
    }
    return false;
}

void control() {
    controls[0].control_number = 1;
    controls[1].control_number = 2;
    controls[2].control_number = 3;
    while (true) {
        for (int i = 0; i < 3; i++) {
            s_sleep(1);
            if (is_limit_reached() || control_stop) {
                break;
            }
            if (controls[i].available_place > 0 and first_in_queue) {
                controls[i].team = (controls[i].available_place == 3) ? none : controls[i].team;
                send_message(first_in_queue, INVITE_TO_CONTROL, controls[i].team);
                listen_for_message_control(&controls[i]);
            }
        }
    }
}
