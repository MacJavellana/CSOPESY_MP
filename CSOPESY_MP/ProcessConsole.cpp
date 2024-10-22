#include "ProcessConsole.h"
#include <iostream>

ProcessConsole::ProcessConsole(std::shared_ptr<Process> process) :
	Console(process->getName()),
	_process(process) {
}

void ProcessConsole::run() {
	if (this->_process->hasFinished()) {
		this->_canRemove = true;
		return;
	}
	system("cls");
	this->_active = true;
	if (this->_history == "") {
		this->draw();
	}
	else {
		std::cout << this->_history;
	}
	std::string input;
	while (this->_active) {
		std::cout << "root:\\> ";
		std::getline(std::cin, input);
		if (input == "exit") {
			this->stop();
			return;
		}
		this->_history += "root:\\> " + input + "\n";
		if (input == "process-smi") {
			this->draw();
			continue;
		}
		if (input == "") {
			continue;
		}
		else {
			std::cout << input << " not recognized." << std::endl;
			this->_history += input + " not recognized.\n";
		}
	}
}

void ProcessConsole::stop() {
	if (this->_process->hasFinished())
		this->_canRemove = true;
	this->_active = false;
}

void ProcessConsole::draw() {
	std::string info = "Process: " + this->_process->getName() + "\n"
		+ "ID: " + std::to_string(this->_process->getID()) + "\n"
		+ "\n";
	if (this->_process->hasFinished()) {
		info += "Finished!\n\n";
	}
	else {
		info += "Current instruction line: " + std::to_string(this->_process->getCommandCounter()) + "\n"
			+ "Lines of code: " + std::to_string(this->_process->getCommandListSize()) + "\n"
			+ "\n";
	}
	std::cout << info;
	this->_history += info;
}