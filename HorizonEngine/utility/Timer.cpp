//Timer.cpp
//Function implementations for the Timer class

#include "Timer.h"

Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

double Timer::GetMillisecondsElapsed()
{
	if (isRunning) {
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else {
		auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}

void Timer::Restart()
{
	isRunning = true;
	start = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop()
{
	if (!isRunning) {
		return false;
	}
	else {
		stop = std::chrono::high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

bool Timer::Start()
{
	if (isRunning) {
		return false;

	}
	else {
		start = std::chrono::high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}