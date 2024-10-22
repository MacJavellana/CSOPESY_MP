#pragma once
#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

#include "Console.h"
#include "ConsoleManager.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

typedef const std::vector<std::string>& argType;


class MainConsole : public Console {
public:
    MainConsole(ConsoleManager* conman);
    ~MainConsole() = default;
    void run() override;
    void stop() override;

private:
    void draw() override;
    void printHeader();

    std::unordered_map<std::string, std::function<void(argType)>> _commandMap;

    bool _initialized = false;

    ConsoleManager* _conman = nullptr;
};

#endif // !MAINCONSOLE_H