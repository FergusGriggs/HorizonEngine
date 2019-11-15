//ErrorLogger.cpp
//Function implementations for the ErrorLogger class

#include "ErrorLogger.h"

void ErrorLogger::Log(std::string message) {
	std::string error_message = "Error: " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::string message) {
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringHelper::StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message) {
	_com_error error(hr);
	std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(COMException& exception)
{
	std::wstring errorMessage = exception.what();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}
