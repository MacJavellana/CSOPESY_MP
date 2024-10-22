#include "PrintCommand.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#define MS_DELAY 50


typedef std::string String;

void PrintCommand::execute(int core, String filename) {
    std::ofstream output;
    output.open(filename, std::ios::out | std::ios::app);

    // output folder must exist!!!
    if (output.is_open()) {
        auto timestamp = time(nullptr);
        struct tm timeInfo;
        localtime_s(&timeInfo, &timestamp);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "(%D %r)", &timeInfo);

        output << buffer << " "
            << "Core:" << core << " "
            << "\"" << this->_message << "\""
            << std::endl;
        output.close();
    }
    else {
        std::cerr << filename << " failed to open" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_DELAY));
}
