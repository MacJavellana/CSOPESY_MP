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
    bool preemptive = stoi(config["preemptive"]) == 1;
    float batch_process_freq = stof(config["batch-process-freq"]);
    int min_ins = stoi(config["min-ins"]);
    int max_ins = stoi(config["max-ins"]);
    float delay_per_exec = stof(config["delay-per-exec"]);

    this->_numCpu = num_cpu;
    this->_scheduler = scheduler;
    this->_quantumCycle = quantum_cycles;
    this->_preemptive = preemptive;
    this->_batchProcessFreq = batch_process_freq;
    this->_minIns = min_ins;
    this->_maxIns = max_ins;
    this->_delaysPerExec = delay_per_exec;
}