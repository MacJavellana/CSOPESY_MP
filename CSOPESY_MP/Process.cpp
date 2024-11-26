#include "Process.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <windows.h>

#include "PrintCommand.h"
#include <mutex>
#include <random>

typedef std::string String;

int Process::requiredPages = -1;
int Process::sameMemory = -1;

Process::Process(String name, std::uniform_int_distribution<int> commandDistr,
    std::uniform_int_distribution<int> memoryDistr,
    std::uniform_int_distribution<int> pageDistr) : _name(name) {

    std::lock_guard<std::mutex> lock(mtx);
    this->_pid = Process::nextID;
    Process::nextID++;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    int numCommands = commandDistr(generator);
    for (int i = 0; i < numCommands; i++) {
        this->_commandList.push_back(
            std::make_shared<PrintCommand>(
                "Hello world from " + this->_name + "!", this->_pid
            )
        );
    }
    if (Process::sameMemory == -1) {
        this->_requiredMemory = memoryDistr(generator);
        int power = 1;
        while (power < this->_requiredMemory) {
            power *= 2;
        }
        this->_requiredMemory = power;
    }
    else {
        this->_requiredMemory = Process::sameMemory;
    }
}

void Process::execute() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!this->hasFinished()) {
        this->_commandList.at(_commandCounter)->execute(this->_cpuCoreID, ".\\output\\" + this->_name + ".txt");
        this->_commandCounter++;
    }
}

bool Process::hasFinished() {
    if (this->_commandCounter >= this->_commandList.size()) {
        return true;
    }
    return false;
}

int Process::setRequiredPages(int min, int max) {
    if (Process::requiredPages == -1) {
        std::uniform_int_distribution<int>  pageDistr(min, max);
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        Process::requiredPages = pageDistr(generator);
        int power = 1;
        while (power < Process::requiredPages) {
            power *= 2;
        }
        Process::requiredPages = power;
    }
    return Process::requiredPages;
}

int Process::setRequiredMemory(int min, int max) {
    if (Process::sameMemory == -1) {
        std::uniform_int_distribution<int>  memDistr(min, max);
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        Process::sameMemory = memDistr(generator);
        int power = 1;
        while (power < Process::sameMemory) {
            power *= 2;
        }
        Process::sameMemory = power;
    }
    return Process::sameMemory;
}

void Process::setCPUCoreID(int cpuCoreID) {
    std::lock_guard<std::mutex> lock(mtx);
    this->_cpuCoreID = cpuCoreID;
}

int Process::nextID = 0;