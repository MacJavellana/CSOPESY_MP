#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <memory>

#include "ICommand.h"
#include <random>


class Process {
public:
    Process(std::string name, std::uniform_int_distribution<int> commandDistr);
    ~Process() = default;

    void execute();
    bool hasFinished();

    int getID() const { return _pid; };
    std::string getName() const { return _name; };
    int getCommandCounter() const { return _commandCounter; };
    int getCommandListSize() const { return _commandList.size(); };
    int getBurst() { return this->getCommandListSize() - this->getCommandCounter(); };
    time_t getArrivalTime() const { return _arrivalTime; };
    time_t getFinishTime() { return _finishTime; };

    void setCPUCoreID(int cpuCoreID);
    void setFinishTime() { this->_finishTime = time(nullptr); };

    bool operator<(std::shared_ptr<Process> other) {
        return this->getBurst() > other->getBurst();
    };
    static int nextID;

    size_t getRequiredMemorySize();
    void* getMemoryAddress();
    void setRequiredMemorySize(size_t size);
    void setMemoryAddress(void* ptr);

private:
    int _pid;
    size_t requiredMemorySize = 4096;
    void* memoryAddress = nullptr;

    std::string _name;
    std::vector<std::shared_ptr<ICommand>> _commandList;
    int _commandCounter = 0;
    int _cpuCoreID = -1;
    time_t _arrivalTime = time(nullptr);
    time_t _finishTime = time(nullptr);
};

#endif // !PROCESS_H