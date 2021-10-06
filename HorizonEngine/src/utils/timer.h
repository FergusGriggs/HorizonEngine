
//Simple timer class that can accurately measure differences in time between it's start and stop functions

#pragma once

#include <chrono>

namespace hrzn::utils
{
	class Timer
	{
	public:
		Timer();

		double getMicrosecondsElapsed();
		float  getDeltaTime();
		void   restart();
		bool   stop();
		bool   start();

	private:
		bool m_isRunning = false;

#ifdef _WIN32
		std::chrono::time_point<std::chrono::steady_clock> m_start;
		std::chrono::time_point<std::chrono::steady_clock> m_stop;
#else
		std::chrono::time_point<std::chrono::system_clock> m_start;
		std::chrono::time_point<std::chrono::system_clock> m_stop;
#endif
	};
}
