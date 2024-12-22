#include "fan.h"

void listen_for_messages_queue() {
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
            if (std::stoi(message.info) == none || std::stoi(message.info) == team) {
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
    }
}

void join_queue() {
    send_message(QUEUE, JOIN_TO_QUEUE);
}

int main(){
    for (int i =0; i<10; i++) fork();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 2);
    team = static_cast<Team>(dis(gen));
    std::cout << "Kibic (PID: " << getpid() << ") jest w drużynie " << team << std::endl;
    create_message_queue();
    std::thread listener_thread(listen_for_messages_queue);
    listener_thread.detach();
    join_queue();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
