#include "PrintCommand.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>


typedef std::string String;

int PrintCommand::msDelay = 50;

void PrintCommand::execute(int core, String filename) {
    //std::ofstream output;
    //output.open(filename, std::ios::out | std::ios::app);

    //// output folder must exist!!!
    //if (output.is_open()) {
    //    auto timestamp = time(nullptr);
    //    struct tm timeInfo;
    //    localtime_s(&timeInfo, &timestamp);
    //    char buffer[80];
    //    strftime(buffer, sizeof(buffer), "(%D %r)", &timeInfo);

    //    output << buffer << " "
    //        << "Core:" << core << " "
    //        << "\"" << this->_message << "\""
    //        << std::endl;
    //    output.close();
    //}
    //else {
    //    std::cerr << filename << " failed to open" << std::endl;
    //}
    std::this_thread::sleep_for(std::chrono::milliseconds(PrintCommand::msDelay));
}

void PrintCommand::setMsDelay(int delay) {
    if (delay < 0) return;
    PrintCommand::msDelay = delay;
}
