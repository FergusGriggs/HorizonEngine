//StringHelper.h
//Provides a simple way to convert strings and perform some directory modifications. All methods are static.

#pragma once

#include <string>
#include <algorithm>

class StringHelper
{
public:
	static std::wstring StringToWide(std::string str);
	static std::string GetDirectoryFromPath(const std::string& filePath);
	static std::string GetFileExtension(const std::string& fileName);
};