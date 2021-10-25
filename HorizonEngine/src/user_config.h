#pragma once

class UserConfig
{
public:
	~UserConfig();

	static UserConfig& it();

	int getWindowWidth() const;
	int getWindowHeight() const;

	const char* getWindowName() const;

private:
	UserConfig();

private:
	int m_windowWidth = 1728;
	int m_windowHeight = 972;

	const char* m_windowName = "HorizonEngine";
};
