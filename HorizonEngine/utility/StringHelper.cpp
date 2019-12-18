//StringHelper.cpp
//Function implementations for the StringHelper class

#include "StringHelper.h"

std::wstring StringHelper::StringToWide(std::string str) {
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filePath)
{
	size_t off1 = filePath.find_last_of('\\');
	size_t off2 = filePath.find_last_of('/');

	if (off1 == std::string::npos && off2 == std::string::npos) {
		return "";
	}

	if (off1 == std::string::npos) {
		return filePath.substr(0, off2);
	}

	if (off2 == std::string::npos) {
		return filePath.substr(0, off1);
	}

	return filePath.substr(0, std::max(off1, off2));
}

std::string StringHelper::GetFileExtension(const std::string& fileName)
{
	size_t off = fileName.find_last_of(".");

	if (off == std::string::npos) {
		return "";
	}

	return std::string(fileName.substr(off + 1));
}

void StringHelper::ReplaceChars(std::string& string, char existingChar, char newChar)
{
	size_t stringSize = string.length();
	for (size_t i = 0; i < stringSize; ++i)
	{
		if (string[i] == existingChar)
		{
			string[i] = newChar;
		}
	}
}
