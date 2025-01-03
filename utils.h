#pragma once
#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fstream>
#include <random>

enum Team {
    none,
    Poland,
    Germany
};

enum FanPlace {
    OnTheWay,
    InQueue,
    OnControl,
    OnTheWayToTheStands,
    OnTheStands,
    OnTheWayToTheRestroom,
    Restroom,
    OnTheWayToEat,
    OnEating
};

enum FIFOAction {
    JOIN_TO_QUEUE, // info is empty
    SET_QUEUED_PROCESS_PID, // info is pid
    INVITE_TO_CONTROL, // info is team
    READY_TO_CONTROL,  // team
    FAN_NERVOUS_ABOUT_WAITING,  // info is empty
    NO_OTHER_IN_QUEUE,  // info is empty
};

struct FIFOMessage {
    long mtype;
    pid_t sender;
    FIFOAction action;
    char info[100];
};

enum FIFOSpecialRecipient {
    CONTROL = 1,
    QUEUE = 2
};

key_t FIFO_KEY;
int FIFO_ID;

void create_message_queue();
void send_message(long, FIFOAction);
void send_message(long, FIFOAction, int);
void send_message(long, FIFOAction, const std::string&);
FIFOMessage receive_message(long mtype);
