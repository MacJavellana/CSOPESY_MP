#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "ICommand.h"
#include <mutex>
#include <random>


class Process {
public:
    Process(std::string name,
        std::uniform_int_distribution<int> commandDistr,
        std::uniform_int_distribution<int> memoryDistr,
        std::uniform_int_distribution<int> pageDistr
    );
    ~Process() = default;

    void execute();
    bool hasFinished();

    int getID() const { return _pid; };
    std::string getName() { std::lock_guard<std::mutex> lock(mtx); return _name; };
    int getCommandCounter() { std::lock_guard<std::mutex> lock(mtx); return _commandCounter; };
    int getCommandListSize() { std::lock_guard<std::mutex> lock(mtx); return _commandList.size(); };
    int getBurst() { return this->getCommandListSize() - this->getCommandCounter(); };
    time_t getArrivalTime() const { return _arrivalTime; };
    time_t getFinishTime() { return _finishTime; };
    int getRequiredMemory() { std::lock_guard<std::mutex> lock(mtx); return _requiredMemory; };
    static int setRequiredPages(int min, int max);
    static int setRequiredMemory(int min, int max);
    static int getRequiredPages() { return Process::requiredPages; };
    int getCPUCoreID() { std::lock_guard<std::mutex> lock(mtx); return _cpuCoreID; };

    void setCPUCoreID(int cpuCoreID);
    void setFinishTime() { this->_finishTime = time(nullptr); };

    bool operator<(std::shared_ptr<Process> other) {
        return this->getBurst() > other->getBurst();
    };
    static int nextID;

private:
    std::mutex mtx;

    int _pid;
    std::string _name;
    std::vector<std::shared_ptr<ICommand>> _commandList;
    int _commandCounter = 0;
    int _cpuCoreID = -1;
    time_t _arrivalTime = time(nullptr);
    time_t _finishTime = time(nullptr);

    int _requiredMemory;
    static int requiredPages;
    static int sameMemory;
};

#endif // !PROCESS_H