//COMException.h
//Primarily defines the COM_ERROR_IF_FAILED macro which will evaluate a HRESULT input and throw an error with extra information if neccessary

#pragma once

#include <comdef.h>
#include "StringHelper.h"

#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__)

class COMException
{
public:
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + StringHelper::StringToWide(file);
		whatmsg += L"\nFunction: " + StringHelper::StringToWide(function);
		whatmsg += L"\nLine: " + StringHelper::StringToWide(std::to_string(line));
	}

	const wchar_t* what() const
	{
		return whatmsg.c_str();
	}

private:
	std::wstring whatmsg;
};