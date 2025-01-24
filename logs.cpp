#include "logs.h"
#include <unistd.h>
#include <iostream>
#pragma once

Logger::Logger(const std::string& baseFileName)
    : mainFileName(baseFileName + ".log"), stopLogging(false) {
    mkdir("logs", 0777);
    fileMain.open(mainFileName, std::ios::app);
    if (!fileMain.is_open()) {
        throw std::runtime_error("Failed to open main log file");
    }

    pidFileName = baseFileName + "/" + std::to_string(getpid()) + ".log";
    mkdir((baseFileName + "/").c_str(), 0777);
    filePid.open(pidFileName, std::ios::app);
    if (!filePid.is_open()) {
        throw std::runtime_error("Failed to open PID log file");
    }

    sem_init(&semMain, 0, 1);
    sem_init(&semPid, 0, 1);

    loggingThread = std::thread(&Logger::processLogs, this);
}

Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopLogging = true;
    }
    cv.notify_all();
    if (loggingThread.joinable() && loggingThread.get_id() != std::this_thread::get_id()) {
        loggingThread.join();
    }

    fileMain.close();
    filePid.close();

    sem_destroy(&semMain);
    sem_destroy(&semPid);
}

Logger& Logger::operator<<(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(message);
    }
    cv.notify_all();
    return *this;
}

void Logger::processLogs() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this]() { return !logQueue.empty() || stopLogging; });

        if (stopLogging && logQueue.empty()) {
            break;
        }

        std::string message = logQueue.front();
        logQueue.pop();
        lock.unlock();

        std::cout << message << std::endl;

        sem_wait(&semMain);
        fileMain << message << std::endl;
        sem_post(&semMain);

        sem_wait(&semPid);
        filePid << message << std::endl;
        sem_post(&semPid);
    }
}

Logger logger("logs/logs");
