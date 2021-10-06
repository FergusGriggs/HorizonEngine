
//Function implementations for the Timer class

#include "Timer.h"

namespace hrzn::utils
{
	Timer::Timer()
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_stop = std::chrono::high_resolution_clock::now();
	}

	double Timer::getMicrosecondsElapsed()
	{
		if (m_isRunning)
		{
			auto elapsed = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - m_start);
			return elapsed.count();
		}
		else
		{
			auto elapsed = std::chrono::duration<double, std::micro>(m_stop - m_start);
			return elapsed.count();
		}
	}

	float Timer::getDeltaTime()
	{
		return static_cast<float>(getMicrosecondsElapsed() * 0.000001);
	}

	void Timer::restart()
	{
		m_isRunning = true;
		m_start = std::chrono::high_resolution_clock::now();
	}

	bool Timer::stop()
	{
		if (!m_isRunning)
		{
			return false;
		}
		else
		{
			m_stop = std::chrono::high_resolution_clock::now();
			m_isRunning = false;
			return true;
		}
	}

	bool Timer::start()
	{
		if (m_isRunning)
		{
			return false;

		}
		else
		{
			m_start = std::chrono::high_resolution_clock::now();
			m_isRunning = true;
			return true;
		}
	}
}
