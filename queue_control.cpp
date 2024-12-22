#include "queue_control.h"

void listen_for_messages() {
    while (true) {
        FIFOMessage message = receive_message(getpid());
        std::cout << "Kolejka otrzymała bezpośrednią wiadomość: "
                  << "Action: " << message.action << ", Sender: " << message.sender
                  << ", Info: " << message.info << std::endl;
        if (message.action == SET_QUEUED_PROCESS_PID) {
            first_in_queue = std::stoi(message.info);
        }
    }
}

void set_queued_process_pid(pid_t to, int info) {
    send_message(to, SET_QUEUED_PROCESS_PID, info);
}

void listen_for_messages_queue() {
    while (true) {
        FIFOMessage message = receive_message(QUEUE);
        std::cout << "Kolejka otrzymała wiadomość: "
                  << "Action: " << message.action << ", Sender: " << message.sender
                  << ", Info: " << message.info << std::endl;
        if (message.action == JOIN_TO_QUEUE) {
            if (last_in_queue)
                send_message(last_in_queue, SET_QUEUED_PROCESS_PID, message.sender);
            last_in_queue = message.sender;
            if (!first_in_queue)
                first_in_queue = message.sender;
        }
    }
}

void listen_for_message_control(Control *control) {
    FIFOMessage message = receive_message(CONTROL);
    std::cout << "Kontrola otrzymała wiadomość: "
              << "Action: " << message.action << ", Sender: " << message.sender
              << ", Info: " << message.info << std::endl;
    if (message.action == READY_TO_CONTROL) {
        control->team = (Team)std::stoi(message.info);
        control->available_place--;
        std::thread worker(&Control::check_fan, control, message.sender);
        worker.detach();
    }
    else if (message.action == FAN_NERVOUS_ABOUT_WAITING) {
        std::cout << "Fan (PID = " << message.sender << " ) is nervous about waiting" << std::endl;
    }
    else if (message.action == NO_OTHER_IN_QUEUE) {
        std::cout << "No other in queue" << std::endl;
    }
}

void Control::check_fan(pid_t fan_pid) {
    std::cout << "Checking fan with PID: " << fan_pid << std::endl;
    std::srand(std::time(nullptr));
    int wait_time = 2 + (std::rand() % 3);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));
    available_place++;
    std::cout << "Fan with PID: " << fan_pid << " has been checked." << std::endl;
}

void control() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (int i = 0; i < 3; i++) {
            std::cout << "Checking control: " << i << " available_place: " << controls[i].available_place << " actual first in queue: " << first_in_queue << std::endl;
            if (controls[i].available_place > 0 and first_in_queue) {
                controls[i].team = (controls[i].available_place == 3) ? none : controls[i].team;
                send_message(first_in_queue, INVITE_TO_CONTROL, controls[i].team);
                listen_for_message_control(&controls[i]);
                break;
            }
        }
    }
}

int main() {
    create_message_queue();
    while (true) {
        FIFOMessage message = {};
        if (msgrcv(FIFO_ID, &message, sizeof(message) - sizeof(long), 0, IPC_NOWAIT) == -1) {
            if (errno == ENOMSG) break;
            else {
                perror("Failed to clear message queue");
                exit(EXIT_FAILURE);
            }
        }else {
            std::cout << "Wiadomość sender: " << message.sender << " Action: " << message.action << std::endl;
        }
    }
    std::thread listener_thread(listen_for_messages);
    listener_thread.detach();
    std::thread listener_queue_thread(listen_for_messages_queue);
    listener_queue_thread.detach();
    std::thread control_thread(control);
    control_thread.detach();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
