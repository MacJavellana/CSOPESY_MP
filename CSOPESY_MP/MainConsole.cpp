
#include <cstdlib>
#include <fstream> // in use
#include <iostream>
#include <memory>
#include <sstream> // in use
#include <string>
#include <vector>

#include "AConsole.h"
#include "Config.h"
#include "ConsoleManager.h"
#include "Cpu.h"
#include "MainConsole.h"
#include "MarqueeConsole.h"
#include "MemoryManager.h"
#include "PrintCommand.h"
#include "Scheduler.h"

#define SPACE " "


MainConsole::MainConsole(ConsoleManager* conman) : AConsole("MAIN_CONSOLE"), _conman(conman) {
	// SCREEN
	this->_commandMap["screen"] = [conman](argType arguments) {
		if (arguments.size() > 2) {
			std::cout << "Too many strings!" << std::endl;
			return;
		}
		else if (arguments.at(0) == "screen") {
			std::cout << "TODO: help for screen" << std::endl;
			return;
		}

		if (arguments.at(0) == "-s") {
			if (arguments.size() == 1)
				std::cout << "No process specified." << std::endl;
			else
				conman->newConsole(arguments.at(1));
		}
		else if (arguments.at(0) == "-r") {
			if (arguments.size() == 1)
				std::cout << "No process specified." << std::endl;
			else
				conman->switchConsole(arguments.at(1));
		}
		else if (arguments.at(0) == "-ls") {
			conman->_scheduler->printStatus();
		}
		else {
			std::cout << "TODO: help for screen" << std::endl;
			return;
		}
		};
	this->_commandMap["marquee"] = [conman](argType arguments) {
		conman->switchConsole("MARQUEE_CONSOLE");
		};
	this->_commandMap["report-util"] = [conman](argType arguments) {
		std::ofstream out("csopesy-log.txt");
		std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
		std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
		conman->_scheduler->printStatus();
		std::cout.rdbuf(coutbuf); //reset to standard output again
		std::cout << "root:\\> Report generated at root:/csopesy-log.txt\n";
		};
	this->_commandMap["scheduler-test"] = [conman](argType arguments) {
		conman->_scheduler->schedulerTest();
		};
	this->_commandMap["scheduler-stop"] = [conman](argType arguments) {
		conman->_scheduler->schedulerTestStop();
		};
	this->_commandMap["memory"] = [conman](argType arguments) {
		conman->_scheduler->printMem();
		};
	this->_commandMap["process-smi"] = [conman](argType arguments) {
		conman->_scheduler->processSmi();
		};
	this->_commandMap["vmstat"] = [conman](argType arguments) {
		conman->_scheduler->vmstat();
		};
}

void MainConsole::run() {
	std::string input;
	while (!this->_initialized) {
		std::cout << "root:\\> ";
		getline(std::cin, input);
		if (input == "initialize") {
			this->_initialized = true;
			this->_conman->newConsole("MARQUEE_CONSOLE", std::make_shared<MarqueeConsole>(144));

			Config config = Config();
			config.initialize();

			if (config.getMinPageProc() != 1 && config.getMaxPageProc() != 1) {
				Process::setRequiredMemory(config.getMinMemProc(), config.getMaxMemProc());
			}

			Scheduler::initialize(config.getNumCpu(),
				config.getBatchProcessFreq(),
				config.getMinIns(), config.getMaxIns(),
				config.getMinMemProc(), config.getMaxMemProc(),
				config.getMaxMem(), config.getMinPageProc(), config.getMaxPageProc());

			Scheduler* sched = Scheduler::get();

			this->_conman->_scheduler = sched;

			PrintCommand::setMsDelay(0);
			CPU::setMsDelay(config.getDelaysPerExec() * 100);

			std::string schedType = config.getScheduler();
			if (schedType == "fcfs") {
				sched->startFCFS(config.getDelaysPerExec());
			}
			else if (schedType == "sjf") {
				sched->startSJF(config.getDelaysPerExec(), config.isPreemptive());
			}
			else if (schedType == "rr") {
				sched->startRR(config.getDelaysPerExec(), config.getQuantumCycle());
			}

		}
		if (input == "exit") {
			this->stop();
			return;
		}
	}
	system("cls");
	this->_active = true;
	this->printHeader();
	while (this->_active) {
		std::cout << "root:\\> ";
		std::getline(std::cin, input);
		std::string command = input.substr(0, input.find(SPACE));
		input.erase(0, input.find(SPACE) + 1);
		if (command == "exit") {
			this->stop();
			return;
		}
		if (this->_commandMap.find(command) == this->_commandMap.end())
			std::cout << "'" << command << "' is not recognized." << std::endl;
		else {
			std::stringstream tokens(input);
			std::vector<std::string> arguments;
			std::string token;
			while (std::getline(tokens, token, ' ')) {
				arguments.push_back(token);
			}
			this->_commandMap[command](arguments);
		}
	}
}

void MainConsole::stop() {
	this->_active = false;
}

void MainConsole::draw() {
}

void MainConsole::printHeader() {
	std::cout << R"(   _____  _____  ____  _____  ______  _______     __)" << std::endl
		<< R"(  / ____|/ ____|/ __ \|  __ \|  ____|/ ____\ \   / /)" << std::endl
		<< R"( | |    | (___ | |  | | |__) | |__  | (___  \ \_/ / )" << std::endl
		<< R"( | |     \___ \| |  | |  ___/|  __|  \___ \  \   /  )" << std::endl
		<< R"( | |____ ____) | |__| | |    | |____ ____) |  | |   )" << std::endl
		<< R"(  \_____|_____/ \____/|_|    |______|_____/   |_|   )" << std::endl
		<< "Hello, Welcome to CSOPSY commandline!" << std::endl
		<< "Type 'exit' to quit, 'clear' to clear the screen" << std::endl;
}