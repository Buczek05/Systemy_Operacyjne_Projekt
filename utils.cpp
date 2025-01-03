#include "utils.h"

#include <thread>

void create_message_queue() {
    FIFO_KEY = ftok(".", 'a');
    if (FIFO_KEY == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }

    FIFO_ID = msgget(FIFO_KEY, 0666 | IPC_CREAT);
    if (FIFO_ID == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }
}

void send_message(long mtype, FIFOAction action) {
    send_message(mtype, action, "");
}

void send_message(long mtype, FIFOAction action, int info) {
    send_message(mtype, action, std::to_string(info).data());
}

void m_send_message(long mtype, FIFOAction action, const std::string& info) {
    FIFOMessage message = {};
    message.mtype = mtype;
    message.sender = getpid();
    message.action = action;
    strncpy(message.info, info.c_str(), sizeof(message.info) - 1);
    message.info[sizeof(message.info) - 1] = '\0';
    if (msgsnd(FIFO_ID, &message, sizeof(message) - sizeof(long), 0) == -1) {
        perror("msgsnd failed");
        exit(EXIT_FAILURE);
    }
}

void send_message(long mtype, FIFOAction action, const std::string& info) {
    std::thread worker(m_send_message, mtype, action, info);
    worker.detach();
}

FIFOMessage receive_message(long mtype) {
    FIFOMessage message = {};
    if (msgrcv(FIFO_ID, &message, sizeof(message) - sizeof(long), mtype, 0) == -1) {
        perror("msgrcv failed");
        exit(EXIT_FAILURE);
    }
    return message;
}


