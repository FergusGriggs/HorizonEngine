//Timer.h
//Simple timer class that can accurately measure differences in time between it's start and stop functions

#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	double GetMillisecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();

private:
	bool isRunning = false;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif
};