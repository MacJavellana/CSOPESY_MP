#pragma once
#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>


enum CommandType {
    PRINT
};

class ICommand {
public:
    virtual void execute(int core, std::string filename) = 0;

    int _pid;
    CommandType _type;
};

#endif // !ICOMMAND_H