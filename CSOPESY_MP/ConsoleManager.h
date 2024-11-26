#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include "AConsole.h"
#include "Scheduler.h"
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include "MainConsole.h"


typedef std::shared_ptr<AConsole> AConsole_;

class ConsoleManager {
public:
    static ConsoleManager* get();
    static void initialize();
    static void destroy();

    void start();
    bool newConsole(std::string name, AConsole_ console = nullptr);
    void switchConsole(std::string processName);

    void setScheduler(Scheduler* scheduler) { _scheduler = scheduler; };

private:
    ConsoleManager();
    ~ConsoleManager();

    static ConsoleManager* ptr;
    std::unordered_map<std::string, AConsole_> _consoleMap;
    AConsole_ _current = nullptr;
    AConsole_ _mainConsole = nullptr;

    Scheduler* _scheduler = nullptr;
    friend class MainConsole;
};

#endif // !CONSOLEMANAGER_H

