#pragma once
#include "fan.h"

void listen_for_messages_fan() {
    pid_t my_pid = getpid();
    while (true) {
        FIFOMessage message = receive_message(my_pid);
        std::ostringstream logStream;
        logStream << "Kibic (PID: " << my_pid << ") otrzymał wiadomość: "
                << "Action: " << message.action << ", Sender: " << message.sender
                << ", Info: " << message.info ;
        logger << logStream.str();
        process_message(message);
    }
}

void process_message(FIFOMessage message) {
    switch (message.action) {
        case SET_QUEUED_PROCESS_PID:
            process_set_queued_process_pid(message);
            break;
        case INVITE_TO_CONTROL:
            process_invite_to_control(message);
            break;
        case ENJOY_THE_GAME:
            process_enjoy_the_game(message);
            break;
        default:
            std::ostringstream logStream;
            logStream << "Unknown action: " << message.action;
            logger << logStream.str();
            break;
    }
}

void process_set_queued_process_pid(FIFOMessage message) {
    std::ostringstream visualization_message;
    visualization_message << "{\"type\": \"set_queued_process_pid\", \"fan_pid\": " << getpid()
            << ", \"queued_process_pid\": " << message.info << "}";
    send_to_visualization(visualization_message.str());
    queued_process_pid = std::stoi(message.info);
}

void process_invite_to_control(FIFOMessage message) {
    if (children_count && std::stoi(message.info) == none) {
        m_send_message(message.sender, SET_QUEUED_PROCESS_PID, std::to_string(queued_process_pid));
        send_message(CONTROL, READY_TO_CONTROL_WITH_CHILDREN, team);
    } else if (std::stoi(message.info) == none || std::stoi(message.info) == team) {
        m_send_message(message.sender, SET_QUEUED_PROCESS_PID, std::to_string(queued_process_pid));
        send_message(CONTROL, READY_TO_CONTROL, team);
    } else if (other_fan_let_count < 5 && queued_process_pid) {
        other_fan_let_count++;
        send_message(queued_process_pid, INVITE_TO_CONTROL, message.info);
    } else if (other_fan_let_count == 5) {
        send_message(CONTROL, FAN_NERVOUS_ABOUT_WAITING);
    } else if (!queued_process_pid) {
        send_message(CONTROL, NO_OTHER_IN_QUEUE);
    }
}

void process_enjoy_the_game(FIFOMessage message) {
    std::ostringstream logStream;
    logStream << "Kibic (PID: " << getpid() << ") cieszy się grą.";
    logger << logStream.str();
    change_location();
}

void join_queue() {
    place = InQueue;
    if (VIP) {
        send_message(STADIUM, VIP_ENTERED_TO_STADIUM, children_count);
    } else {
        send_message(STADIUM, JOIN_TO_QUEUE);
    }
}

void generate_children() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<> children_distribution({80, 15, 5});
    children_count = children_distribution(gen);

    std::uniform_int_distribution<> age_distribution(0, 15);

    children = new Child[children_count];
    child_threads = new std::thread[children_count];
    for (int i = 0; i < children_count; i++) {
        int age = age_distribution(gen);
        children[i] = Child(age);
        child_threads[i] = std::thread(create_child);
        child_threads[i].detach();
    }
}

void setup_random_fan_data() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 2);
    team = static_cast<Team>(dis(gen));
    VIP = std::uniform_int_distribution<>(1, 200)(gen) == 1;
    age = std::uniform_int_distribution<>(16, 100)(gen);
    generate_children();
    std::ostringstream logStream;
    logStream << "Kibic (PID: " << getpid() << "), wiek: " << age << ", VIP: " << (VIP ? "TAK" : "NIE")
            << ", drużyna: " << team << ", liczba dzieci: " << children_count << ", dzieci: ";
    for (int i = 0; i < children_count; i++) {
        if (i > 0) logStream << ", ";
        logStream << "dziecko " << i + 1 << " (wiek: " << children[i].age << " lat)";
    }
    logger << logStream.str();
}

bool is_outside() {
    return place == OnTheWay || place == InQueue || place == OnControl;
}

void checking_evacuation() {
    while (is_outside() || *evacuation_signal == 0) {
        s_sleep(1);
    }
    std::ostringstream logStream;
    logStream << "Fan " << getpid() << " is evacuating.";
    logger << logStream.str();
    std::random_device rd;
    std::mt19937 gen(rd());
    move_to(Leaving, Leaved);
    send_message(STADIUM, LEAVING_STADIUM);
}

void move_to(const FanPlace moving_place, const FanPlace destination) {
    std::ostringstream visualization_message;
    visualization_message << "{\"type\": \"moving\", \"fan_pid\": " << getpid()
            << ", \"from\": " << place << ", \"to\": " << destination << "}";
    send_to_visualization(visualization_message.str());

    int is_evacuating = *evacuation_signal;
    std::ostringstream logStream;
    logStream << "Kibic (PID: " << getpid() << ") zmienia miejsce z " << place << " na " << destination;
    logger << logStream.str();
    place = moving_place;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10, 60);
    int wait_time = dis(gen);
    s_sleep(wait_time);
    if (is_evacuating == *evacuation_signal) {
        place = destination;
        logStream.str("");
        logStream << "Kibic (PID: " << getpid() << ") dotarł na miejsce: " << place;
        logger << logStream.str();
    }

    visualization_message.str("");
    visualization_message << "{\"type\": \"moved\", \"fan_pid\": " << getpid()
            << ", \"place\": " << destination << "}";
    send_to_visualization(visualization_message.str());
}

void change_location() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> evacuation_time(1, 100);
    int chance = evacuation_time(gen);
    if (chance < 80 && place != OnTheStands) move_to(OnTheWayToTheStands, OnTheStands);
    else if (chance < 80 && place != Restroom) move_to(OnTheWayToTheRestroom, Restroom);
    else if (place != OnEating) move_to(OnTheWayToEat, OnEating);
    else move_to(OnTheWayToTheRestroom, Restroom);
}

void change_location_if_want() {
    if (*evacuation_signal) return;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    int chance = dis(gen);
    if (
        (place == OnTheStands && chance % (20 * 60) == 0)
        || (place == Restroom && chance % (60) == 0)
        || (place == OnEating && chance % (5 * 60) == 0)
    ) {
        change_location();
    }
}

void create_child() {
    while (true) {
        s_sleep(1);
        change_location_if_want();
    }
}


void fan(){
    new(&logger) Logger("logs/fan");
    setup_random_fan_data();
    create_message_queue();
    create_evacuation_shared_memory();
    std::thread evacuation_thread(checking_evacuation);
    evacuation_thread.detach();
    std::thread listener_thread(listen_for_messages_fan);
    listener_thread.detach();
    join_queue();
    while (true) {
        s_sleep(1);
        change_location_if_want();
    }
}
