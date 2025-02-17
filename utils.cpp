# pragma once
#include "utils.h"
#include <thread>
#include <sys/shm.h>

#include "logs.h"

extern Logger logger;


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
void clear_queue() {
    while (true) {
        FIFOMessage message = {};
        if (msgrcv(FIFO_ID, &message, sizeof(message) - sizeof(long), 0, IPC_NOWAIT) == -1) {
            if (errno == ENOMSG) break;
            else {
                perror("Failed to clear message queue");
                exit(EXIT_FAILURE);
            }
        }else {
            std::ostringstream logStream;
            logStream << "Wiadomość sender: " << message.sender << " Action: " << message.action;
            logger << logStream.str();
        }
    }
}


void send_message(long mtype, FIFOAction action) {
    send_message(mtype, action, "");
}

void send_message(long mtype, FIFOAction action, int info) {
    send_message(mtype, action, std::to_string(info).data());
}

void send_message(long mtype, FIFOAction action, const std::string& info) {
    std::thread worker(m_send_message, mtype, action, info);
    worker.detach();
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

FIFOMessage receive_message(long mtype) {
    FIFOMessage message = {};
    if (msgrcv(FIFO_ID, &message, sizeof(message) - sizeof(long), mtype, 0) == -1) {
        perror("msgrcv failed");
        exit(EXIT_FAILURE);
    }
    return message;
}

void delete_message_queue() {
    if (msgctl(FIFO_ID, IPC_RMID, nullptr) == -1) {
        perror("msgctl failed");
        exit(EXIT_FAILURE);
    }
}

void create_evacuation_shared_memory() {
    key_t key = ftok("stadium", 65);
    int shm_id = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    evacuation_signal = (int *) shmat(shm_id, NULL, 0);
}

void delete_evacuation_shared_memory() {
    if (shmdt(evacuation_signal) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
    key_t key = ftok("stadium", 65);
    int shm_id = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }
}

void s_sleep(int seconds) {
    if (!testing) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }
    else if (testing_sleep_s) {
        std::this_thread::sleep_for(std::chrono::seconds(testing_sleep_s));
    }
}

void ms_sleep(int ms) {
    if (!testing) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    else if (testing_sleep_s) {
        std::this_thread::sleep_for(std::chrono::seconds(testing_sleep_s));
    }
}

int get_random_number(int from, int to) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(from, to);
    return dis(gen);
}

void send_to_visualization(const std::string& message) {
    send_message(VISUALIZATION, VISUALIZATION_ACTION, message);
}
