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
            if (team == std::stoi(message.info)) {
                send_message(CONTROL, READY_TO_CONTROL);
                send_message(message.sender, SET_QUEUED_PROCESS_PID, queued_process_pid);
            }
            else if (other_fan_let_count < 5 && queued_process_pid) {
                other_fan_let_count++;
                send_message(queued_process_pid, INVITE_TO_CONTROL, message.info);
            }
            else if (other_fan_let_count == 5) {
                send_message(queued_process_pid, FAN_NERVOUS_ABOUT_WAITING);
            }
            else if (!queued_process_pid) {
                send_message(CONTROL, NO_OTHER_IN_QUEUE);
            }
        }
    }
}

void join_queue() {
    send_message(CONTROL, JOIN_TO_QUEUE);
}

int main() {
    team = static_cast<Team>(rand() % 2 + 1);
    create_message_queue();
    std::thread listener_thread(listen_for_messages_queue);
    listener_thread.detach();
    join_queue();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
