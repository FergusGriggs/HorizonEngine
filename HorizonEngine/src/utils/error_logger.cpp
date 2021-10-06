
//Function implementations for the ErrorLogger class

#include "error_logger.h"

#include "string_helpers.h"

namespace hrzn::utils
{
	void ErrorLogger::log(std::string message)
	{
		std::string error_message = "Error: " + message;
		MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);
	}

	void ErrorLogger::log(HRESULT hr, std::string message)
	{
		_com_error error(hr);
		std::wstring error_message = L"Error: " + string_helpers::stringToWide(message) + L"\n" + error.ErrorMessage();
		MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
	}

	void ErrorLogger::log(HRESULT hr, std::wstring message)
	{
		_com_error error(hr);
		std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
		MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
	}

	void ErrorLogger::log(COMException& exception)
	{
		std::wstring errorMessage = exception.what();
		MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
	}
}