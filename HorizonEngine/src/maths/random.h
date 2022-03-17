#pragma once

#include <stdlib.h>
#include <time.h>

// Stop the compiler complaining about SeedWithCurrentTime not being used when it is
#pragma warning(disable : 4505)

namespace hrzn::maths::random
{
	static void seedWithCurrentTime()
	{
		srand((unsigned int)time(nullptr));
	}

	template<typename T>
	static T zeroToOne()
	{
		return (T)(rand()) / (T)(RAND_MAX);
	}

	template<typename T>
	static T negOneToOne()
	{
		return zeroToOne<T>() * (T)2.0 - (T)1.0;
	}

	template<typename T>
	static T inRange(T minVal, T maxVal)
	{
		return minVal + (T)(zeroToOne<double>() * (double)(maxVal - minVal));
	}
}
