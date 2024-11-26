#pragma once
#ifndef PROCESSCONSOLE_H
#define PROCESSCONSOLE_H

#include "AConsole.h"
#include "Process.h"


class ProcessConsole : public AConsole {
public:
    ProcessConsole(std::shared_ptr<Process> process);
    ~ProcessConsole() = default;
    void run() override;
    void stop() override;

private:
    void draw() override;
    std::shared_ptr<Process> _process;
    std::string _history;
};

#endif // !PROCESSCONSOLE_H