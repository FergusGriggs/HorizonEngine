
//Provides a simple way to convert strings and perform some directory modifications. All methods are static.

#pragma once

#include <string>
#include <algorithm>

namespace hrzn::utils::string_helpers
{
	static std::wstring stringToWide(std::string str)
	{
		std::wstring wide_string(str.begin(), str.end());
		return wide_string;
	}

	static std::string getDirectoryFromPath(const std::string& filePath)
	{
		size_t off1 = filePath.find_last_of('\\');
		size_t off2 = filePath.find_last_of('/');

		if (off1 == std::string::npos && off2 == std::string::npos)
		{
			return "";
		}

		if (off1 == std::string::npos)
		{
			return filePath.substr(0, off2);
		}

		if (off2 == std::string::npos)
		{
			return filePath.substr(0, off1);
		}

		return filePath.substr(0, std::max(off1, off2));
	}

	static std::string getFileExtension(const std::string& fileName)
	{
		size_t off = fileName.find_last_of(".");

		if (off == std::string::npos)
		{
			return "";
		}

		return std::string(fileName.substr(off + 1));
	}

	static void removeDirectoriesFromStart(std::string& filePath, int numDirectories)
	{
		int numDirectoriesFound = 0;
		size_t strLength = filePath.size();
		for (size_t i = 0; i < strLength; ++i)
		{
			if (filePath[i] == '\\' || filePath[i] == '/')
			{
				if (++numDirectoriesFound >= numDirectories)
				{
					filePath = filePath.substr(i + 1, strLength - 1 - i);
					return;
				}
			}
		}
	}

	static void replaceChars(std::string& string, char existingChar, char newChar)
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
}