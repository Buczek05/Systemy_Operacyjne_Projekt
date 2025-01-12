#include "fan.h"

void listen_for_messages_stadium() {
    pid_t my_pid = getpid();
    while (true) {
        FIFOMessage message = receive_message(my_pid);
        std::cout << "Kibic (PID: " << my_pid << ") otrzymał wiadomość: "
                  << "Action: " << message.action << ", Sender: " << message.sender
                  << ", Info: " << message.info << std::endl;
        if (message.action == SET_QUEUED_PROCESS_PID) {
            queued_process_pid = std::stoi(message.info);
        }
        else if (message.action == INVITE_TO_CONTROL) {
            if (children_count && std::stoi(message.info) == none) {
                m_send_message(message.sender, SET_QUEUED_PROCESS_PID, std::to_string(queued_process_pid));
                send_message(CONTROL, READY_TO_CONTROL_WITH_CHILDREN, team);
            }
            else if (std::stoi(message.info) == none || std::stoi(message.info) == team) {
                m_send_message(message.sender, SET_QUEUED_PROCESS_PID, std::to_string(queued_process_pid));
                send_message(CONTROL, READY_TO_CONTROL, team);
            }
            else if (other_fan_let_count < 5 && queued_process_pid) {
                other_fan_let_count++;
                send_message(queued_process_pid, INVITE_TO_CONTROL, message.info);
                std::cout << "Kibic (PID: " << my_pid << ") zaprosił innego kibica (PID: " << queued_process_pid << ") do kontroli. Aktualnie przepuścił " << other_fan_let_count << " kibiców" << std::endl;
            }
            else if (other_fan_let_count == 5) {
                send_message(CONTROL, FAN_NERVOUS_ABOUT_WAITING);
            }
            else if (!queued_process_pid) {
                send_message(CONTROL, NO_OTHER_IN_QUEUE);
            }
        }
        else if (message.action == ENJOY_THE_GAME) {
            std::cout << "Kibic (PID: " << my_pid << ") cieszy się grą." << std::endl;
            place = OnTheWayToTheStands;
        }
    }
}

void join_queue() {
    if (VIP) {
        send_message(STADIUM, VIP_ENTERED_TO_STADIUM, children_count);
    }
    else {
        send_message(STADIUM, JOIN_TO_QUEUE, VIP);
    }
}

void generate_children() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<> children_distribution({80, 15, 5});
    children_count = children_distribution(gen);

    std::uniform_int_distribution<> age_distribution(0, 15);

    children = new Child[children_count];
    for (int i = 0; i < children_count; i++) {
        int age = age_distribution(gen);
        children[i] = Child(age);
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
    std::cout << "Kibic (PID: " << getpid() << "), wiek: " << age << ", VIP: " << (VIP ? "TAK" : "NIE")
            << ", drużyna: " << team << ", liczba dzieci: " << children_count << ", dzieci: ";
    for (int i = 0; i < children_count; i++) {
        if (i > 0) std::cout << ", ";
        std::cout << "dziecko " << i + 1 << " (wiek: " << children[i].age << " lat)";
    }
    std::cout << std::endl;
}

int is_outside() {
    return place == OnTheWay || place == InQueue || place == OnControl;
}

void checking_evacuation() {
    while (is_outside() || *evacuation_signal == 0) {
        sleep(1);
    }
    printf("Fan %d is evacuating.\n", getpid());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> evacuation_time(2, 120);
    int wait_time = evacuation_time(gen);
    sleep(wait_time);
    send_message(STADIUM, LEAVING_STADIUM);
}

int main(){
    setup_random_fan_data();
    create_message_queue();
    create_evacuation_shared_memory();
    std::thread evacuation_thread(checking_evacuation);
    evacuation_thread.detach();
    std::thread listener_thread(listen_for_messages_stadium);
    listener_thread.detach();
    join_queue();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
