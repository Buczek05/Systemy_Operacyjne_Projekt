#include <iostream>
#include <thread>
#include "fan.cpp"

int main(){
    setup_random_fan_data();
    create_message_queue();
    create_evacuation_shared_memory();
    std::thread evacuation_thread(checking_evacuation);
    evacuation_thread.detach();
    std::thread listener_thread(listen_for_messages);
    listener_thread.detach();
    join_queue();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        change_location_if_want();
    }
    return 0;
}
