#pragma once
#include <string>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <sys/stat.h>

class Logger {
private:
    std::ofstream fileMain;
    std::ofstream filePid;
    std::string mainFileName;
    std::string pidFileName;
    sem_t semMain;
    sem_t semPid;
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool stopLogging;
    std::thread loggingThread;

    void processLogs();

public:
    explicit Logger(const std::string& baseFileName);
    ~Logger();

    Logger& operator<<(const std::string& message);
};
