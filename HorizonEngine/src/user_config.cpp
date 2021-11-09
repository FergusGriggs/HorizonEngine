#include "user_config.h"

UserConfig::UserConfig() :
	m_windowWidth(1728),
	m_windowHeight(972),

	m_windowWidthFloat(static_cast<float>(m_windowWidth)),
	m_windowHeightFloat(static_cast<float>(m_windowHeight)),

	m_windowName("HorizonEngine")
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

float UserConfig::getWindowWidthFloat() const
{
	return m_windowWidthFloat;
}

float UserConfig::getWindowHeightFloat() const
{
	return m_windowHeightFloat;
}

float UserConfig::getWindowAspectRatio() const
{
	return m_windowWidthFloat / m_windowHeightFloat;
}

const char* UserConfig::getWindowName() const
{
	return m_windowName;
}
