#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include "Console.h"
#include "Scheduler.h"
#include <chrono>
#include <string>
#include <unordered_map>
#include "MainConsole.h"


typedef std::shared_ptr<Console> Console_;

class ConsoleManager {
public:
    static ConsoleManager* get();
    static void initialize();
    static void destroy();

    void start();
    bool newConsole(std::string name, Console_ console = nullptr);
    void switchConsole(std::string processName);

    void setScheduler(Scheduler* scheduler) { _scheduler = scheduler; };

private:
    ConsoleManager();
    ~ConsoleManager();

    static ConsoleManager* ptr;
    std::unordered_map<std::string, Console_> _consoleMap;
    Console_ _current = nullptr;
    Console_ _mainConsole = nullptr;

    Scheduler* _scheduler = nullptr;
    friend class MainConsole;
};

#endif // !CONSOLEMANAGER_H

