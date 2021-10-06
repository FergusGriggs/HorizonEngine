
//Provides a simple way to create error windows, automatically evaluating hr (HRESULT) input into a string message.

#pragma once

#include <Windows.h>

#include "com_exception.h"

namespace hrzn::utils
{
	class ErrorLogger
	{
	public:
		static void log(std::string message);
		static void log(HRESULT hr, std::string message);
		static void log(HRESULT hr, std::wstring message);
		static void log(COMException& exception);
	};
}
