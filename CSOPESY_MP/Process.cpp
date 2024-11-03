#include "Process.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <windows.h>

#include "PrintCommand.h"
#include <random>

typedef std::string String;

Process::Process(String name, std::uniform_int_distribution<int> commandDistr) : _name(name) {
    //const char* dirPath = "output";

    //DWORD attribs = GetFileAttributesA(dirPath);
    //if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
    //    CreateDirectoryA(dirPath, NULL);
    //}

    //String filename = ".\\output\\" + this->_name + ".txt";
    //std::ofstream output;
    //output.open(filename, std::ios::out);
    //if (output.is_open()) {
    //    output << "Process name: " << this->_name << std::endl
    //        << "Logs:" << std::endl
    //        << std::endl;
    //    output.close();
    //}
    //if (filler) {
    //    for (int i = 0; i < 100; i++) {
    //        this->_commandList.push_back(
    //            std::make_shared<PrintCommand>(
    //                "Hello world from " + this->_name + "!", this->_pid
    //            )
    //        );
    //    }
    //}
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
}

void Process::execute() {
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

void Process::setCPUCoreID(int cpuCoreID) {
    this->_cpuCoreID = cpuCoreID;
}

int Process::nextID = 0;