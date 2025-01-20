#pragma once
#include "queue.h"

void listen_for_messages_queue() {
    while (true) {
        FIFOMessage message = receive_message(getpid());
        std::ostringstream logStream;
        logStream << "Kolejka otrzymała bezpośrednią wiadomość: "
                << "Action: " << message.action << ", Sender: " << message.sender
                << ", Info: " << message.info;
        logger << logStream.str();
        if (message.action == SET_QUEUED_PROCESS_PID) {
            first_in_queue = std::stoi(message.info);
        }
    }
}

void set_queued_process_pid(pid_t to, int info) {
    send_message(to, SET_QUEUED_PROCESS_PID, info);
}

void process_join_to_queue(FIFOMessage message) {
    std::ostringstream visualization_message;
    visualization_message << "{\"type\": \"join_to_queue\", \"fan_pid\": " << message.sender << "}";
    send_to_visualization(visualization_message.str());

    if (last_in_queue)
        send_message(last_in_queue, SET_QUEUED_PROCESS_PID, message.sender);
    last_in_queue = message.sender;
    if (!first_in_queue)
        first_in_queue = message.sender;
}

void queue() {
  listen_for_messages_queue();
}