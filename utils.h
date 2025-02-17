#pragma once
#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fstream>
#include <random>
#include <sstream>
#include <queue>

enum Team {
    none,
    Poland,
    Germany
};

enum FanPlace {
// OUTSIDE
    OnTheWay,
    InQueue,
    OnControl,
    Leaved,
// INSIDE
    OnTheWayToTheStands,
    OnTheStands,
    OnTheWayToTheRestroom,
    Restroom,
    OnTheWayToEat,
    OnEating,
    Leaving,
};

enum FIFOAction {
    JOIN_TO_QUEUE,
    VIP_ENTERED_TO_STADIUM, // info is children_count
    SET_QUEUED_PROCESS_PID, // info is pid
    INVITE_TO_CONTROL, // info is team
    READY_TO_CONTROL,  // team
    READY_TO_CONTROL_WITH_CHILDREN,  // children_count
    FAN_NERVOUS_ABOUT_WAITING,  // info is empty
    NO_OTHER_IN_QUEUE,  // info is empty
    ENJOY_THE_GAME,  // info is empty
    LIMIT_REACHED,  // info is empty
    LEAVING_STADIUM,  // info is empty
    VISUALIZATION_ACTION,  // info is message
};

struct FIFOMessage {
    long mtype;
    pid_t sender;
    FIFOAction action;
    char info[100];
};

enum FIFOSpecialRecipient {
    CONTROL = 1,
    STADIUM = 2,
    VISUALIZATION = 3,
};

key_t FIFO_KEY;
int FIFO_ID;
bool testing = false;
int testing_sleep_s = 0;

void create_message_queue();
void clear_queue();
void send_message(long, FIFOAction);
void send_message(long, FIFOAction, int);
void send_message(long, FIFOAction, const std::string&);
void m_send_message(long, FIFOAction, const std::string&);
FIFOMessage receive_message(long mtype);
void delete_message_queue();

int *evacuation_signal;
void create_evacuation_shared_memory();
void delete_evacuation_shared_memory();


void s_sleep(int seconds);
void ms_sleep(int ms);

int get_random_number(int from, int to);

void send_to_visualization(const std::string& message);