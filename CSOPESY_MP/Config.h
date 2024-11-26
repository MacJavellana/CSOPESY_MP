#ifndef CONFIG_H
#define CONFIG_H
#include "Scheduler.h"

#include <string>

using namespace std;

class Config
{
public:
    Config();
    ~Config();
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

    int getMaxMem() const {
        return _maxMem;
    }

    int getMinMemProc() const {
        return _minMemProc;
    }

    int getMaxMemProc() const {
        return _maxMemProc;
    }

    float getDelaysPerExec() const {
        return _delaysPerExec;
    }

    int getMinPageProc() const {
        return _minPageProc;
    }

    int getMaxPageProc() const {
        return _maxPageProc;
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
    int _maxMem;
    int _minMemProc;
    int _maxMemProc;
    float _delaysPerExec;
    int _minPageProc;
    int _maxPageProc;
    bool running = false;

    friend class Scheduler;
};

#endif // !CONFIG_H