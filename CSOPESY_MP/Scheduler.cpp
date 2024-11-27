#include "Config.h"
#include "Scheduler.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include "CPU.h"
#include "MemoryManager.h"
#include "Process.h"

Scheduler::Scheduler() {}

int Scheduler::getTotalTicks() {
    int total = 0;
    for (size_t i = 0; i < this->_cpuList.size(); i++) {
        total += _cpuList[i]->getTotalTicks();
    }
    return total;
}

int Scheduler::getInactiveTicks() {
    int total = 0;
    for (size_t i = 0; i < this->_cpuList.size(); i++) {
        total += _cpuList[i]->getInactiveTicks();
    }
    return total;
}

Scheduler* Scheduler::_ptr = nullptr;

Scheduler* Scheduler::get() {
    return _ptr;
}

void Scheduler::initialize(int cpuCount,
    float batchProcessFreq,
    int minIns, int maxIns,
    int minMemProc, int maxMemProc,
    int maxMem, int minPage, int maxPage) {
    _ptr = new Scheduler();
    for (int i = 0; i < cpuCount; i++) {
        _ptr->_cpuList.push_back(std::make_shared<CPU>());
    }
    _ptr->batchProcessFreq = batchProcessFreq;
    _ptr->minIns = minIns;
    _ptr->maxIns = maxIns;
    _ptr->_minMemProc = minMemProc;
    _ptr->_maxMemProc = maxMemProc;
    _ptr->_minPage = minPage;
    _ptr->_maxPage = maxPage;
    _ptr->_memMan = new MemoryManager(maxMem, minPage, maxPage);
}

void Scheduler::startFCFS(int delay) {
    if (this->running == false) {
        this->running = true;
        std::thread t(&Scheduler::runFCFS, this, delay);
        t.detach();
    }
}
void Scheduler::startSJF(int delay, bool preemptive) {
    if (this->running == false) {
        this->running = true;
        std::thread t(&Scheduler::runSJF, this, delay, preemptive);
        t.detach();
    }
}

void Scheduler::startRR(int delay, int quantumCycles) {
    if (this->running == false) {
        this->running = true;
        std::thread t(&Scheduler::runRR, this, delay, quantumCycles);
        t.detach();
    }
}

void Scheduler::stop() {
    this->running = false;
}

void Scheduler::destroy() {
    delete _ptr;
}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(this->mtx);
    if (Config::_scheduler == "sjf") {
        this->_readyQueueSJF.push(process);
    }
    else {
        this->_readyQueue.push(process);
    }
    this->_processList.push_back(process);
}

void Scheduler::printStatus() {
    std::lock_guard<std::mutex> lock(this->mtx);
    int cpuReadyCount = 0;
    for (std::shared_ptr<CPU> cpu : this->_cpuList) {
        if (cpu->isReady()) {
            cpuReadyCount++;
        }
    }
    float cpuUtilization = 100.0 * (this->_cpuList.size() - cpuReadyCount) / this->_cpuList.size();

    std::cout << "CPU Utilization: " << cpuUtilization << "%" << std::endl
        << "Cores used: " << this->_cpuList.size() - cpuReadyCount << std::endl
        << "Cores available: " << cpuReadyCount << std::endl
        << std::endl;

    for (int i = 0; i < 38; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
    std::cout << "Running processes:" << std::endl;
    for (int i = 0; i < this->_cpuList.size(); i++) {
        std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
        if (cpu->isReady()) {
            std::cout << "Idle\tCore: " << std::to_string(cpu->getId()) << std::endl;
        }
        else {
            std::string process = cpu->getProcessName();
            std::string commandCounter = std::to_string(cpu->getProcessCommandCounter());
            std::string totalCommands = std::to_string(cpu->getProcessCommandListSize());
            std::string cpuID = std::to_string(cpu->getId());

            auto timestamp = cpu->getProcessArrivalTime();
            struct tm timeInfo;
            localtime_s(&timeInfo, &timestamp);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "(%D %r)", &timeInfo);

            std::cout << process + "\t" + buffer + "\t" + "Core: " + cpuID + "\t" + commandCounter + " / " + totalCommands << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "Finished processes:" << std::endl;

    for (int i = 0; i < this->_processList.size(); i++) {
        if (this->_processList.at(i)->hasFinished()) {
            std::string process = this->_processList.at(i)->getName();
            std::string commandCounter = std::to_string(this->_processList.at(i)->getCommandCounter());
            std::string totalCommands = std::to_string(this->_processList.at(i)->getCommandListSize());

            auto timestamp = this->_processList.at(i)->getFinishTime();
            struct tm timeInfo;
            localtime_s(&timeInfo, &timestamp);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "(%D %r)", &timeInfo);

            std::cout << process + "\t" + buffer + "\t" + "Finished" + "\t" + commandCounter + " / " + totalCommands << std::endl;
        }
    }
    for (int i = 0; i < 38; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
}

void Scheduler::printMem() {
    this->_memMan->printMem(this->_cycleCount);
}

void Scheduler::schedulerTest() {
    std::cout << "Started adding processes." << std::endl;
    this->_testRunning = true;
    std::thread t(&Scheduler::schedulerRun, this);
    t.detach();
}

void Scheduler::schedulerRun() {
    std::uniform_int_distribution<int>  commandDistr(this->minIns, this->maxIns);
    std::uniform_int_distribution<int>  memDistr(this->_minMemProc, this->_maxMemProc);
    std::uniform_int_distribution<int>  pageDistr(this->_minPage, this->_maxPage);
    while (this->_testRunning) {
        std::shared_ptr<Process> process = std::make_shared<Process>("process_" + std::to_string(Process::nextID), commandDistr, memDistr, pageDistr);
        this->addProcess(process);
        std::this_thread::sleep_for(std::chrono::milliseconds(int(this->batchProcessFreq * 100)));
    }

}

void Scheduler::schedulerTestStop() {
    this->_testRunning = false;
    std::cout << "Stopped adding processes." << std::endl;
}

void Scheduler::runFCFS(float delay) { // FCFS
    while (this->running) {
        for (int i = 0; i < this->_cpuList.size(); i++) {
            std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
            if (cpu->isReady()) {
                if (cpu->getProcess() != nullptr && cpu->getProcess()->hasFinished()) {
                    _memMan->deallocate(cpu->getProcess());
                }
                if (this->_readyQueue.size() > 0) {
                    if (_memMan->allocate(this->_readyQueue.front())) {
                        cpu->setProcess(this->_readyQueue.front());
                        this->_readyQueue.pop();
                        this->running = true;
                    }
                }
            }
            //else {
            //    if (this->running == false) {
            //        std::chrono::duration<float> duration(delay);
            //        std::this_thread::sleep_for(duration);
            //        this->running = true;
            //    }
            //}
        }
    }
}

void Scheduler::runSJF(float delay, bool preemptive) { // SJF
    std::unique_lock<std::mutex> lock(this->mtx);
    lock.unlock();
    if (preemptive) {
        while (this->running) {
            lock.lock();
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                std::shared_ptr<Process> oldProcess = cpu->getProcess();
                if (oldProcess != nullptr && oldProcess->hasFinished()) {
                    _memMan->deallocate(oldProcess);
                }
                cpu->setProcess(nullptr);

                if (oldProcess != nullptr && !oldProcess->hasFinished()) this->_readyQueueSJF.push(oldProcess);

                if (!this->_readyQueueSJF.empty()) {
                    std::shared_ptr<Process> newProcess = this->_readyQueueSJF.top();
                    this->_readyQueueSJF.pop();
                    if (_memMan->allocate(newProcess)) {
                        cpu->setProcess(newProcess);
                    }
                    else {
                        this->_readyQueueSJF.push(newProcess);
                    }
                }
            }
            lock.unlock();
        }
    }
    else {
        while (this->running) {
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->isReady()) {
                    if (cpu->getProcess() != nullptr && cpu->getProcess()->hasFinished()) {
                        _memMan->deallocate(cpu->getProcess());
                        cpu->setProcess(nullptr);
                    }
                    if (this->_readyQueueSJF.size() > 0) {
                        if (_memMan->allocate(this->_readyQueueSJF.top())) {
                            cpu->setProcess(this->_readyQueueSJF.top());
                            this->_readyQueueSJF.pop();
                            this->running = true;
                        }
                    }
                }
                //else {
                //    if (this->running == false) {
                //        std::chrono::duration<float> duration(delay);
                //        std::this_thread::sleep_for(duration);
                //        this->running = true;
                //    }
                //}
            }
        }
    }
}

void Scheduler::runRR(float delay, int quantumCycles) { // RR
    auto start = std::chrono::steady_clock::now();
    this->_cycleCount = 0;
    while (this->running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        // Check if quantum cycle limit exceeded

        if (elapsed > quantumCycles) {
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->getProcess() != nullptr) {
                    // Push current process back to ready queue
                    //_memMan->deallocate(cpu->getProcess()); // TO UNCOMMENT
                    this->_readyQueue.push(cpu->getProcess());
                    cpu->setProcess(nullptr);
                    cpu->setReady();
                    this->running = true; // Set running to true to continue scheduling
                }
            }
            start = std::chrono::steady_clock::now(); // Reset start time for new cycle
            this->_cycleCount++;
        }

        // Assign processes to CPUs
        for (int i = 0; i < this->_cpuList.size(); i++) {
            std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
            if (cpu->getProcess() != nullptr && cpu->getProcess()->hasFinished()) {
                _memMan->deallocate(cpu->getProcess());
                cpu->setProcess(nullptr);
                cpu->setReady();
            }
            if (cpu->isReady() && !this->_readyQueue.empty()) {
                std::shared_ptr<Process> process = this->_readyQueue.front();

                if (_memMan->allocate(process)) {
                    process->setCPUCoreID(cpu->getId());
                    cpu->setProcess(process);
                    this->_readyQueue.pop();
                    this->running = true;
                }
                else {
                    this->_readyQueue.pop();
                    this->_readyQueue.push(process);
                }
                start = std::chrono::steady_clock::now(); // Reset start time for the new process
            }
        }

        //// If no tasks were scheduled, sleep for delay
        //if (!this->running) {
        //    std::chrono::duration<float> duration(delay);
        //    std::this_thread::sleep_for(duration);
        //    this->running = true; // Set running to true to continue scheduling
        //}
    }
}

void Scheduler::processSmi() {
    for (int i = 0; i < 48; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;

    std::cout << "| PROCESS-SMI V01.00 \t Driver Version: 01.00 |" << std::endl;

    for (int i = 0; i < 48; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;

    int cpuUse = 100 / this->_cpuList.size();
    int cpuUsage = 0;
    for (int i = 0; i < this->_cpuList.size(); i++) {
        if (this->_cpuList[i]->getProcess() != nullptr) {
            cpuUsage += cpuUse;
        }
    }

    std::cout << "CPU-Util: " << cpuUsage << "%" << std::endl;

    this->_memMan->getAllocator()->printProcesses();
}

void Scheduler::vmstat() {
    this->_memMan->vmstat();
}


