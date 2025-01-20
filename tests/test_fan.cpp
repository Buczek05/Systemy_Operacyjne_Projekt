#include "../fan.cpp"
#include "utils.cpp"

bool test_listen_for_messages() {
    create_message_queue();
    std::thread listener_thread(listen_for_messages_fan);
    listener_thread.detach();
    queued_process_pid = 0;
    send_message(getpid(), SET_QUEUED_PROCESS_PID, 123111);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    listener_thread.~thread();
    return queued_process_pid == 123111;
}

bool test_process_message_set_queued_process_pid() {
    FIFOMessage message = {};
    message.action = SET_QUEUED_PROCESS_PID;
    strncpy(message.info, "123111", sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    queued_process_pid = 0;
    process_message(message);
    return queued_process_pid == 123111;
}

bool test_process_message_invite_to_control_1() {
    FIFOMessage message = {};
    message.action = INVITE_TO_CONTROL;
    message.sender = 111;
    strncpy(message.info, std::to_string(none).c_str(), sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    children_count = 1;
    process_message(message);
    FIFOMessage set_queued_process_pid_message = receive_message(111);
    FIFOMessage control_message = receive_message(CONTROL);
    if (set_queued_process_pid_message.action != SET_QUEUED_PROCESS_PID) throw TestException(
        "set_queued_process_pid_message.action != SET_QUEUED_PROCESS_PID");
    if (std::stoi(set_queued_process_pid_message.info) != queued_process_pid) throw TestException(
        "std::stoi(set_queued_process_pid_message.info) != queued_process_pid");
    if (control_message.action != READY_TO_CONTROL_WITH_CHILDREN) throw TestException(
        "control_message.action != READY_TO_CONTROL_WITH_CHILDREN");
    if (std::stoi(control_message.info) != team) throw TestException("std::stoi(control_message.info) != team");
    return true;
}


bool test_process_message_invite_to_control_2() {
    FIFOMessage message = {};
    message.action = INVITE_TO_CONTROL;
    message.sender = 111;
    team = Poland;
    for (int i = 0; i < 2; i++) {
        strncpy(message.info, std::to_string(i).c_str(), sizeof(message.info) - 1);
        message.info[sizeof(message.info) - 1] = '\0';
        children_count = 0;
        process_message(message);
        FIFOMessage set_queued_process_pid_message = receive_message(111);
        FIFOMessage control_message = receive_message(CONTROL);
        if (set_queued_process_pid_message.action != SET_QUEUED_PROCESS_PID) throw TestException(
            "set_queued_process_pid_message.action != SET_QUEUED_PROCESS_PID");
        if (std::stoi(set_queued_process_pid_message.info) != queued_process_pid) throw TestException(
            "std::stoi(set_queued_process_pid_message.info) != queued_process_pid");
        if (control_message.action != READY_TO_CONTROL) throw TestException(
            "control_message.action != READY_TO_CONTROL");
        if (std::stoi(control_message.info) != team) throw TestException("std::stoi(control_message.info) != team");
    }
    return true;
}


bool test_process_message_invite_to_control_3() {
    FIFOMessage message = {};
    message.action = INVITE_TO_CONTROL;
    message.sender = getpid();
    strncpy(message.info, std::to_string(Poland).c_str(), sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    team = Germany;
    other_fan_let_count = 0;
    queued_process_pid = 123111;
    process_message(message);
    FIFOMessage queued_process_pid_message = receive_message(queued_process_pid);
    if (queued_process_pid_message.action != INVITE_TO_CONTROL) throw TestException(
    "queued_process_pid_message.action != INVITE_TO_CONTROL");
    if (std::stoi(queued_process_pid_message.info) != Poland) throw TestException(
        "std::stoi(queued_process_pid_message.info) != Poland");
    return other_fan_let_count == 1;
}

bool test_process_message_invite_to_control_4() {
    FIFOMessage message = {};
    message.action = INVITE_TO_CONTROL;
    message.sender = getpid();
    strncpy(message.info, std::to_string(Poland).c_str(), sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    team = Germany;
    other_fan_let_count = 5;
    process_message(message);
    FIFOMessage control_message = receive_message(CONTROL);
    if (control_message.action != FAN_NERVOUS_ABOUT_WAITING) throw TestException(
        "control_message.action != FAN_NERVOUS_ABOUT_WAITING");
    return true;
}

bool test_process_message_invite_to_control_5() {
    FIFOMessage message = {};
    message.action = INVITE_TO_CONTROL;
    message.sender = getpid();
    strncpy(message.info, std::to_string(Poland).c_str(), sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    team = Germany;
    other_fan_let_count = 0;
    queued_process_pid = 0;
    process_message(message);
    FIFOMessage control_message = receive_message(CONTROL);
    if (control_message.action != NO_OTHER_IN_QUEUE) throw TestException(
        "control_message.action != NO_OTHER_IN_QUEUE");
    return true;
}

bool test_process_message_enjoy_the_game() {
    FIFOMessage message = {};
    place = InQueue;
    message.action = ENJOY_THE_GAME;
    process_message(message);
    if (place == InQueue) throw TestException("place == InQueue");
    return true;
}

bool test_join_queue() {
    clear_queue();
    VIP = false;
    join_queue();
    FIFOMessage message = receive_message(STADIUM);
    if (message.action != JOIN_TO_QUEUE) throw TestException("message.action != JOIN_TO_QUEUE");

    VIP = true;
    children_count = 5;
    join_queue();
    FIFOMessage vip_message = receive_message(STADIUM);
    if (vip_message.action != VIP_ENTERED_TO_STADIUM) throw TestException("vip_message.action != VIP_ENTERED_TO_STADIUM");
    if (std::stoi(vip_message.info) != children_count) throw TestException("std::stoi(vip_message.info) != children_count");
    return true;
}

bool test_generate_children() {
    generate_children();
    return true;
}

bool test_setup_random_fan_data() {
    team = none;
    setup_random_fan_data();
    if (team != Poland && team != Germany) throw TestException("team != Poland && team != Germany");
    return true;
}

bool test_is_outside() {
    FanPlace outside[3] = {OnTheWay, InQueue, OnControl};
    FanPlace inside[6] = {OnTheWayToTheStands, OnTheStands, OnTheWayToTheRestroom, Restroom, OnTheWayToEat, OnEating};
    for (int i = 0; i < 3; i++) {
        place = outside[i];
        if (!is_outside()) throw TestException("is_outside() failed for place: " + std::to_string(place));
    }
    for (int i = 0; i < 6; i++) {
        place = inside[i];
        if (is_outside()) throw TestException("!is_outside() failed for place: " + std::to_string(place));
    }
    return true;
}

bool test_checking_evacuation() {
    place = OnTheWay;
    std::thread worker(checking_evacuation);
    worker.detach();
    *evacuation_signal = 1;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (place != OnTheWay) throw TestException("place != OnTheWay");
    *evacuation_signal = 0;
    place = OnTheStands;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (place != OnTheStands) throw TestException("place != OnTheStands");
    *evacuation_signal = 1;
    FIFOMessage message = receive_message(STADIUM);
    if (place != Leaved) throw TestException("place != Leaved");
    if (message.action != LEAVING_STADIUM) throw TestException("message.action != LEAVING_STADIUM");
    return true;
}

bool test_move_to() {
    place = InQueue;
    testing_sleep_s = 1;
    std::thread worker(move_to, OnTheWayToTheStands, OnTheStands);
    worker.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (place != OnTheWayToTheStands) throw TestException("place != OnTheWayToTheStands");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (place != OnTheStands) throw TestException("place != OnTheStands");
    testing_sleep_s = 0;
    return true;
}

bool test_change_location() {
    place = InQueue;
    change_location();
    if (place != OnTheStands && place != Restroom && place != OnEating) throw TestException(
        "place != OnTheStands || place != Restroom || place != OnEating");
    FanPlace actual_place = place;
    change_location();
    if (place == actual_place) throw TestException("place == actual_place");
    return true;
}

