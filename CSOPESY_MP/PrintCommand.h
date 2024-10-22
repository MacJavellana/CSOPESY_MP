#pragma once
#ifndef PRINTCOMMAND_H
#define PRINTCOMMAND_H

#include "ICommand.h"
#include <string>


class PrintCommand : public ICommand {
public:
    PrintCommand(std::string message, int pid) : _message(message), _pid(pid) {};
    ~PrintCommand() = default;

    void execute(int core, std::string output);

    int _pid;
    CommandType _type = PRINT;

    std::string _message;
};

#endif // !PRINTCOMMAND_H