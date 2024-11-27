#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

Config::Config() {

}

Config::~Config() {

}

std::string Config::_scheduler = "";

void Config::initialize() {
    ifstream inputFile("config.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return;
    }

    unordered_map<string, string> config;
    string line, key, value;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        iss >> key >> value;
        config[key] = value;
    }

    inputFile.close();

    int num_cpu = stoi(config["num-cpu"]);
    string scheduler = config["scheduler"].substr(1, config["scheduler"].size() - 2); // Remove quotes
    int quantum_cycles = stoi(config["quantum-cycles"]);
    bool preemptive = 1;
    float batch_process_freq = stof(config["batch-process-freq"]);
    int min_ins = stoi(config["min-ins"]);
    int max_ins = stoi(config["max-ins"]);
    int max_mem = stoi(config["max-overall-mem"]);
    int min_mem_proc = stoi(config["min-mem-per-proc"]);
    int max_mem_proc = stoi(config["max-mem-per-proc"]);
    float delay_per_exec = stof(config["delay-per-exec"]);
    int min_page_proc = stoi(config["min-page-per-proc"]);
    int max_page_proc = stoi(config["max-page-per-proc"]);


    this->_numCpu = num_cpu;
    this->_scheduler = scheduler;
    this->_quantumCycle = quantum_cycles;
    this->_preemptive = preemptive;
    this->_batchProcessFreq = batch_process_freq;
    this->_minIns = min_ins;
    this->_maxIns = max_ins;
    this->_maxMem = max_mem;
    this->_minMemProc = min_mem_proc;
    this->_maxMemProc = max_mem_proc;
    this->_delaysPerExec = delay_per_exec;
    this->_minPageProc = min_page_proc;
    this->_maxPageProc = max_page_proc;
}