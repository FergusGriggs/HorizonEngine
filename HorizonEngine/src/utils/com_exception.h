
//Primarily defines the COM_ERROR_IF_FAILED macro which will evaluate a HRESULT input and throw an error with extra information if neccessary

#pragma once

#include <comdef.h>
#include "string_helpers.h"

#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw hrzn::utils::COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__)

namespace hrzn::utils
{
	class COMException
	{
	public:
		COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
		{
			_com_error error(hr);
			whatmsg = L"Msg: " + string_helpers::stringToWide(std::string(msg)) + L"\n";
			whatmsg += error.ErrorMessage();
			whatmsg += L"\nFile: " + string_helpers::stringToWide(file);
			whatmsg += L"\nFunction: " + string_helpers::stringToWide(function);
			whatmsg += L"\nLine: " + string_helpers::stringToWide(std::to_string(line));
		}

		const wchar_t* what() const
		{
			return whatmsg.c_str();
		}

	private:
		std::wstring whatmsg;
	};
}