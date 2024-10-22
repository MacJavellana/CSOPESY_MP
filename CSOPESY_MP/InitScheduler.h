#ifndef INITSCHEDULER_H
#define INITSCHEDULER_H
#include "Scheduler.h"

#include <string>

using namespace std;

class InitScheduler
{
public:
    InitScheduler();
    ~InitScheduler();
    void initialize();

    int getNumCpu() const {
        return _numCpu;
    }

    std::string getScheduler() const {
        return _scheduler;
    }

    int getQuantumCycle() const {
        return _quantumCycle;
    }

    bool isPreemptive() const {
        return _preemptive;
    }

    float getBatchProcessFreq() const {
        return _batchProcessFreq;
    }

    int getMinIns() const {
        return _minIns;
    }

    int getMaxIns() const {
        return _maxIns;
    }

    float getDelaysPerExec() const {
        return _delaysPerExec;
    }

    bool isRunning() const {
        return running;
    }
private:
    int _numCpu;
    static string _scheduler;
    int _quantumCycle;
    bool _preemptive;
    float _batchProcessFreq;
    int _minIns;
    int _maxIns;
    float _delaysPerExec;
    bool running = false;

    friend class Scheduler;
};

#endif