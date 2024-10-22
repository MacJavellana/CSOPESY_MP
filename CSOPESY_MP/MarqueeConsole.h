#pragma once
#ifndef MARQUEECONSOLE_H
#define MARQUEECONSOLE_H

#include "Console.h"
#include <string>

class MarqueeConsole : public Console {
public:
	MarqueeConsole(int refreshRate);
	~MarqueeConsole() = default;

	void run();
	void stop();

private:
	void draw();
	void processCommand();

	bool stopFlag = true;
	int refreshRate;
	int interval;

	std::string input = "";
	std::string previous = "";
};

#endif // !MARQUEECONSOLE_H
