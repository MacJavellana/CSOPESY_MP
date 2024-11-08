#include "Console.h"
#include "ConsoleManager.h"
#include "MainConsole.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include "Scheduler.h"
#include "MarqueeConsole.h"
#include "Config.h"
#include "PrintCommand.h"
#include "MemoryAllocator.h"
#include <random>

#define SPACE " "


MainConsole::MainConsole(ConsoleManager* conman) : Console("MAIN_CONSOLE"), _conman(conman) {
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
}

void MainConsole::run() {
	std::string input;
	while (!this->_initialized) {
		std::cout << "root:\\> ";
		getline(std::cin, input);
		if (input == "initialize") {
			this->_initialized = true;
			this->_conman->newConsole("MARQUEE_CONSOLE", std::make_shared<MarqueeConsole>(144));

			Config schedConfig = Config();
			schedConfig.initialize();
			Scheduler::initialize(schedConfig.getNumCpu(), schedConfig.getBatchProcessFreq(), schedConfig.getMinIns(), schedConfig.getMaxIns(), std::make_shared<MemoryAllocator>(16384));

			Scheduler* sched = Scheduler::get();

			this->_conman->_scheduler = sched;

			PrintCommand::setMsDelay(schedConfig.getDelaysPerExec() * 100);

			std::string schedType = schedConfig.getScheduler();
			if (schedType == "fcfs") {
				sched->startFCFS(schedConfig.getDelaysPerExec());
			}
			else if (schedType == "sjf") {
				sched->startSJF(schedConfig.getDelaysPerExec(), schedConfig.isPreemptive());
			}
			else if (schedType == "rr") {
				sched->startRR(schedConfig.getDelaysPerExec(), schedConfig.getQuantumCycle());
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