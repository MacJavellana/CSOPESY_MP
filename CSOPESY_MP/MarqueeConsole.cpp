#include "Console.h"
#include "MarqueeConsole.h"
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <tuple>


MarqueeConsole::MarqueeConsole(int refreshRate) : Console("MarqueeConsole") {
	this->refreshRate = refreshRate;
	this->interval = 1000.0 / refreshRate;
}

void MarqueeConsole::run() {
	if (this->stopFlag != true)
		return;


	char DELETE = 8;
	char ENTER = 13;
	this->stopFlag = false;
	this->_active = true;
	int key = 0;

	std::thread outputThread(&MarqueeConsole::draw, this);
	outputThread.detach();

	while (!this->stopFlag) {
		if (_kbhit()) {
			key = _getch();
			if (key == DELETE) {
				if (this->input.size() > 0) this->input.pop_back();
			}
			else if (key == ENTER) {
				processCommand();
				this->input = "";
			}
			else {
				this->input.push_back(key);
			}
		}
	}
}

void MarqueeConsole::stop() {
	stopFlag = true;
}

void MarqueeConsole::draw() {
	std::string message = "Hello world in marquee!";

	std::tuple<int, int> bounds = getWindowSize();
	int top = 3;
	int bottom = std::get<1>(bounds) - 3;
	int right = std::get<0>(bounds) - message.size();
	int left = 0;
	int x = left;
	int y = top;
	bool moveUp = false;
	bool moveLeft = false;

	while (!this->stopFlag) {
		system("cls");

		for (int i = 0; i < 41; i++) std::cout << '*';
		std::cout << std::endl;
		std::cout << "* Displaying a marquee console! *" << std::endl;
		for (int i = 0; i < 41; i++) std::cout << '*';
		std::cout << std::endl;

		std::string::difference_type n = std::count(this->previous.begin(), this->previous.end(), '\n');

		PrintAtCoords(y, x, message);
		SetCursorPosition(bottom + 1, left);
		std::cout << this->previous;
		std::cout << "root\\marquee:\\> " << this->input << std::endl;
		SetCursorPosition(bottom + 1 + n, left + 16 + this->input.size());

		if (moveUp) {
			y--;
			if (y <= top) moveUp = false;
		}
		else {
			y++;
			if (y >= bottom) moveUp = true;
		}

		if (moveLeft) {
			x--;
			if (x <= left) moveLeft = false;
		}
		else {
			x++;
			if (x >= right) moveLeft = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(this->interval));
	}
	this->_active = false;
}

void MarqueeConsole::processCommand() {
	if (this->input == "exit") {
		this->previous = "";
		this->stop();
	}
	else if (this->input == "") {
		this->previous += "root\\marquee:\\> \n";
	}
	else {
		this->previous += "root\\marquee:\\> " + this->input + "\n";
		this->previous += "Command processed: " + this->input + "\n";
	}
}

