#include "user_config.h"

UserConfig::UserConfig()
{
}

UserConfig::~UserConfig()
{
}

UserConfig& UserConfig::it()
{
	static UserConfig s;
	return s;
}

int UserConfig::getWindowWidth() const
{
	return m_windowWidth;
}

int UserConfig::getWindowHeight() const
{
	return m_windowHeight;
}

const char* UserConfig::getWindowName() const
{
	return m_windowName;
}
