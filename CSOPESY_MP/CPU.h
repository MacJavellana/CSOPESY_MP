#pragma once
#ifndef CPU_H
#define CPU_H

#include <memory>
#include <string>

#include "Process.h"
#include <mutex>


class CPU {
public:
    CPU();
    ~CPU() = default;
    void setProcess(std::shared_ptr<Process> process);
    int getId() const { return _id; };
    std::string getProcessName() const { return this->_process->getName(); };
    int getProcessCommandCounter() const { return this->_process->getCommandCounter(); };
    int getProcessCommandListSize() const { return this->_process->getCommandListSize(); };
    time_t getProcessArrivalTime() const { return this->_process->getArrivalTime(); };
    std::shared_ptr<Process> getProcess() { return this->_process; };
    int getTotalTicks() { return this->_totalTicks; };
    int getInactiveTicks() { return this->_inactiveTicks; };

    void stop() { this->_stopFlag = true; };
    bool isReady() const { return _ready; };
    void setReady() { this->_ready = true; };
    static void setMsDelay(int delay) { CPU::msDelay = delay; };

private:
    void run();
    void execute();

    std::mutex mtx;
    static int msDelay;

    static int nextID;
    int _id;
    bool _ready = true;
    bool _stopFlag = false;
    int _inactiveTicks = 0;
    int _totalTicks = 0;

    std::shared_ptr<Process> _process = nullptr;
};

#endif // !CPU_H