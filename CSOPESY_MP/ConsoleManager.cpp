#include "Console.h"
#include "ConsoleManager.h"
#include <iostream>
#include <string>
#include "MainConsole.h"
#include "ProcessConsole.h"


ConsoleManager* ConsoleManager::ptr = nullptr;

ConsoleManager* ConsoleManager::get() {
    return ConsoleManager::ptr;
}

void ConsoleManager::initialize() {
    ConsoleManager::ptr = new ConsoleManager();
}

void ConsoleManager::destroy() {
    ConsoleManager::ptr = nullptr;
}

void ConsoleManager::start() {
    this->_current->run();
}

bool ConsoleManager::newConsole(std::string name, Console_ console) {
    if (this->_consoleMap.find(name) != this->_consoleMap.end()) {
        std::cout << "Process '" + name + "' is already running!" << std::endl;
        return false;
    }

    bool found = false;
    if (console == nullptr) {
        std::vector<std::shared_ptr<Process>> copyList = this->_scheduler->_processList;
        for (int i = 0; i < copyList.size(); i++) {
            if (name == copyList.at(i)->getName() && !copyList.at(i)->hasFinished()) {
                console = std::make_shared<ProcessConsole>(copyList.at(i));
                found = true;
                break;
            }
        }
        if (found)
            this->_consoleMap[name] = console;

        this->switchConsole(name);
    }
    else {
        this->_consoleMap[name] = console;
    }

    return true;
}

void ConsoleManager::switchConsole(std::string processName) {
    if (this->_consoleMap.find(processName) == this->_consoleMap.end()) {
        std::cout << "Process " + processName + " not found." << std::endl;
        return;
    }
    else if (this->_consoleMap[processName]->canRemove()) {
        this->_consoleMap.erase(processName);
        std::cout << "Process " + processName + " not found." << std::endl;
        return;
    }

    this->_current->stop();

    this->_current = this->_consoleMap[processName];
    this->_current->run();

    // Wait for console to set active to false
    while (this->_current->isActive()) {}

    if (this->_current->canRemove())
        this->_consoleMap.erase(processName);

    this->_current = this->_mainConsole;
    this->_current->run();
}

ConsoleManager::ConsoleManager() {
    this->_mainConsole = std::make_shared<MainConsole>(MainConsole(this));
    this->_current = this->_mainConsole;
}

ConsoleManager::~ConsoleManager() {}
