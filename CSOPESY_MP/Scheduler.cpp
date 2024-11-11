#include "Scheduler.h"
#include "Config.h"

#include <ctime>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <limits.h>

#include "CPU.h"
#include "Process.h"
#include "MemoryAllocator.h"

#include <fstream>
#include <filesystem>
#include <iomanip>

std::string generateTimestamp() {
    auto now = chrono::system_clock::now();
    std::time_t now_time = chrono::system_clock::to_time_t(now);
    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now_time);
#else
    localtime_r(&now_time, &local_tm);
#endif
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &local_tm);
    return std::string(buffer);
}

Scheduler::Scheduler() {

}

Scheduler* Scheduler::_ptr = nullptr;

Scheduler* Scheduler::get() {
    return _ptr;
}

void Scheduler::initialize(int cpuCount, float batchProcessFreq, int minIns, int maxIns, std::shared_ptr<MemoryAllocator> memoryAllocator) {
    _ptr = new Scheduler();
    for (int i = 0; i < cpuCount; i++) {
        _ptr->_cpuList.push_back(std::make_shared<CPU>());
    }
    _ptr->batchProcessFreq = batchProcessFreq;
    _ptr->minIns = minIns;
    _ptr->maxIns = maxIns;
    _ptr->allocator = memoryAllocator;
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
    if (Config::_scheduler == "sjf") {
        this->_readyQueueSJF.push(process);
    }
    else {
        this->_readyQueue.push(process);
    }
    this->_processList.push_back(process);
}

void Scheduler::printStatus() {
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

void Scheduler::printMemory(int quantumCycle) const {
    int allocationCount = allocator->getAllocationCount();
    size_t fragmentedSpace = allocator->getFragmentedSpace();

    std::ostringstream filenameStream;
    filenameStream << "memory_stamp_" << std::setw(2) << std::setfill('0') << quantumCycle << ".txt";
    std::string filename = filenameStream.str();

    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    outFile << "Timestamp: " << generateTimestamp() << "\n";

    outFile << "Number of processes in memory: " << allocationCount << "\n";

    outFile << "Total external fragmentation in KB: " << fragmentedSpace << "\n";

    outFile << "\n";

    outFile << allocator->printMemory();

    outFile.close();
}

void Scheduler::schedulerTest() {
    this->_testRunning = true;
    std::thread t(&Scheduler::schedulerRun, this);
    t.detach();
}

void Scheduler::schedulerRun() {
    std::cout << "Started adding processes." << std::endl;
    while (this->_testRunning) {
        std::uniform_int_distribution<int>  distr(this->minIns, this->maxIns);
        std::shared_ptr<Process> process = std::make_shared<Process>("process_" + std::to_string(Process::nextID), distr);
        this->addProcess(process);
        std::this_thread::sleep_for(std::chrono::milliseconds(int(this->batchProcessFreq * 1000)));
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
                if (this->_readyQueue.size() > 0) {
                    cpu->setProcess(this->_readyQueue.front());
                    this->_readyQueue.pop();
                    this->running = true;
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
    if (preemptive) {
        while (this->running) {
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->isReady()) {
                    if (!this->_readyQueueSJF.empty()) {
                        cpu->setProcess(this->_readyQueueSJF.top());
                        this->_readyQueueSJF.pop();
                        this->running = true;
                    }
                }
                else {
                    if (this->running == true && !this->_readyQueueSJF.empty()) {
                        if (cpu->getProcess()->getBurst() > this->_readyQueueSJF.top()->getBurst()) {
                            std::chrono::duration<float> duration(delay);
                            std::this_thread::sleep_for(duration);
                            this->_readyQueueSJF.push(cpu->getProcess());
                            cpu->setProcess(this->_readyQueueSJF.top());
                            this->_readyQueueSJF.pop();
                        }
                    }
                    //if (this->running == false) {
                    //    std::chrono::duration<float> duration(delay);
                    //    std::this_thread::sleep_for(duration);
                    //    this->running = true;
                    //}
                }
            }
        }
    }
    else {
        while (this->running) {
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->isReady()) {
                    if (this->_readyQueueSJF.size() > 0) {
                        cpu->setProcess(this->_readyQueueSJF.top());
                        this->_readyQueueSJF.pop();
                        this->running = true;
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

/*
void Scheduler::runRR(float delay, int quantumCycles, std::shared_ptr<MemoryAllocator> allocator) { // RR
    auto start = std::chrono::steady_clock::now();
    while (this->running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        // Check if quantum cycle limit exceeded
        if (elapsed > quantumCycles) {
            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->getProcess() != nullptr) {
                    // Push current process back to ready queue
                    this->_readyQueue.push(cpu->getProcess());
                    cpu->setProcess(nullptr);
                    cpu->setReady();
                    this->running = true; // Set running to true to continue scheduling
                }
            }
            start = std::chrono::steady_clock::now(); // Reset start time for new cycle
        }

        // Assign processes to CPUs
        for (int i = 0; i < this->_cpuList.size(); i++) {
            std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
            if (cpu->isReady() && !this->_readyQueue.empty()) {
                cpu->setProcess(this->_readyQueue.front());
                this->_readyQueue.pop();
                this->running = true;
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
*/
void Scheduler::runRR(float delay, int quantumCycles) {
    auto start = std::chrono::steady_clock::now();
    int quantumCycle = 0; // Initialize quantum cycle counter

    // Log memory state immediately when the scheduler starts
    //printMemory(quantumCycle);

    while (this->running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        // Check if quantum cycle limit exceeded
        if (elapsed > quantumCycles) {
            quantumCycle++;  // Increment quantum cycle counter
            printMemory(quantumCycle);  // Log memory state at the start of each quantum cycle

            for (int i = 0; i < this->_cpuList.size(); i++) {
                std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
                if (cpu->getProcess() != nullptr) {
                    std::shared_ptr<Process> process = cpu->getProcess();

                    // Deallocate memory if process is being preempted
                    allocator->deallocate(process->getMemoryAddress(), process->getRequiredMemorySize());
                    process->setMemoryAddress(nullptr);

                    // Push the current process back to the ready queue
                    this->_readyQueue.push(process);
                    cpu->setProcess(nullptr);
                    cpu->setReady();
                    this->running = true;
                }
            }
            start = std::chrono::steady_clock::now(); // Reset start time for the new quantum cycle
        }

        // Assign processes to CPUs if there are available processes
        for (int i = 0; i < this->_cpuList.size(); i++) {
            std::shared_ptr<CPU> cpu = this->_cpuList.at(i);
            if (cpu->isReady() && !this->_readyQueue.empty()) {
                std::shared_ptr<Process> currentProcess = this->_readyQueue.front();

                // Attempt to allocate memory for the process
                void* memory = allocator->allocate(currentProcess->getRequiredMemorySize());
                if (memory != nullptr) {
                    currentProcess->setMemoryAddress(memory);
                    this->_readyQueue.pop();
                    cpu->setProcess(currentProcess);
                    this->running = true;
                    start = std::chrono::steady_clock::now(); // Reset start time for this process assignment
                }
                else {
                    // Break if there is insufficient memory and defer to the next cycle
                    break;
                }
            }
        }
    }
}



