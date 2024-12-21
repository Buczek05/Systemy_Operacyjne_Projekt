#include "queue_control.h"

void listen_for_messages() {
    while (true) {
        FIFOMessage message = receive_message(getpid());
        std::cout << "Kolejka otrzymała bezpośrednią wiadomość: "
                  << "Action: " << message.action << ", Sender: " << message.sender
                  << ", Info: " << message.info << std::endl;
    }
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

void listen_for_message_control(Control &control) {
    FIFOMessage message = receive_message(CONTROL);
    std::cout << "Kontrola otrzymała wiadomość: "
              << "Action: " << message.action << ", Sender: " << message.sender
              << ", Info: " << message.info << std::endl;
    if (message.action == READY_TO_CONTROL) {
        control.available_place--;
        std::thread worker([&control, fan_pid = message.sender]() {
            control.check_fan(fan_pid);
        });
        worker.detach();
    }
    else if (message.action == FAN_NERVOUS_ABOUT_WAITING || message.action == NO_OTHER_IN_QUEUE) {
        return;
    }
}

void Control::check_fan(pid_t fan_pid) {
    std::cout << "Checking fan with PID: " << fan_pid << std::endl;
    std::srand(std::time(nullptr));
    int wait_time = 2 + (std::rand() % 59);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));
    available_place++;
    std::cout << "Fan with PID: " << fan_pid << " has been checked." << std::endl;
}

int main() {
    create_message_queue();
    std::thread listener_thread(listen_for_messages_queue);
    listener_thread.detach();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (int i=0; i<3; i++) {
            if (controls[i].available_place > 0 and first_in_queue) {
                controls[i].team = (controls[i].available_place == 3) ? none : controls[i].team;
                send_message(first_in_queue, INVITE_TO_CONTROL, controls[i].team);
                listen_for_message_control(controls[i]);
            }
        }
    }
    return 0;
}
