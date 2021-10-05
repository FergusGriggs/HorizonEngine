//ErrorLogger.h
//Provides a simple way to create error windows, automatically evaluating hr (HRESULT) input into a string message.

#pragma once

#include <Windows.h>

#include "COMException.h"
#include "StringHelper.h"

class ErrorLogger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(COMException& exception);
};

