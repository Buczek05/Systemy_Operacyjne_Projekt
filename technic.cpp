#include "technic.h"

void handle_exit_signal(int signal) {
    logger << "Exiting...";
    delete_message_queue();
    delete_evacuation_shared_memory();
    exit(0);
}

void stop_control(int sig) {
    control_stop = !control_stop;
    if (control_stop) {
        logger << "Kontrola zatrzymana";
    } else {
        logger << "Kontrola wznowiona";
    }
}

void start_evacuation(int sig) {
    *evacuation_signal = 1;
    control_stop = true;
    int fan_inside = 0;
    while (true) {
        s_sleep(10);
        int current_fan_inside = fan_inside_control.get_inside_fans_count();
        if (current_fan_inside == 0) {
            logger << "Wszyscy kibice opuścili stadion";
            break;
        }
        if (fan_inside == current_fan_inside) {
            std::ostringstream logStream;
            logStream << "Zagubieni kibice podczas ewakuacji:";
            fan_inside_control.print_fans_inside(logStream);
            logger << logStream.str();
        }
        fan_inside = current_fan_inside;
    }
}

void listen_for_messages_stadium() {
    while (true) {
        FIFOMessage message = receive_message(STADIUM);
        std::ostringstream logStream;
        logStream << "Kolejka otrzymała wiadomość: "
                << "Action: " << message.action << ", Sender: " << message.sender
                << ", Info: " << message.info;
        logger << logStream.str();
        if (message.action == JOIN_TO_QUEUE) {
            process_join_to_queue(message);
        } else if (message.action == VIP_ENTERED_TO_STADIUM) {
            int children_count = std::stoi(message.info);
            fan_inside_control.add_fan_inside(message.sender, children_count + 1);
            send_message(message.sender, ENJOY_THE_GAME);
        } else if (message.action == LEAVING_STADIUM) {
            fan_inside_control.remove_fan_inside(message.sender);
        }
    }
}

void technic() {
    new(&logger) Logger("logs/technic");
    logger << "Technic started";
    signal(SIGUSR1, stop_control);
    signal(SIGUSR2, start_evacuation);
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);

    create_message_queue();
    clear_queue();
    create_evacuation_shared_memory();
    *evacuation_signal = 0;

    std::thread queue_thread(queue);
    queue_thread.detach();
    std::thread message_listener(listen_for_messages_stadium);
    message_listener.detach();
    std::thread control_thread(control);
    control_thread.detach();

    while (true) {
        s_sleep(1);
    }
}
