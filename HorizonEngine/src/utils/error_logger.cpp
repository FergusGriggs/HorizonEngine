
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

	void ErrorLogger::log(COMException& exception)
	{
		std::wstring errorMessage = exception.what();
		MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
	}

	void ErrorLogger::log(HRESULT hr, std::string message)
	{
		log(hr, string_helpers::stringToWide(message));
	}

	void ErrorLogger::log(HRESULT hr, std::wstring message)
	{
		_com_error error(hr);
		std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();
		MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
	}

	bool ErrorLogger::logIfFailed(HRESULT hr, std::string message)
	{
		return logIfFailed(hr, string_helpers::stringToWide(message));
	}

	bool ErrorLogger::logIfFailed(HRESULT hr, std::wstring message)
	{
		if (FAILED(hr))
		{
			log(hr, message);

			return true;
		}

		return false;
	}
}
