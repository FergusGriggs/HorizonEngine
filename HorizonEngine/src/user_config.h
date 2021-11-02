#pragma once

class UserConfig
{
public:
	~UserConfig();

	static UserConfig& it();

	int getWindowWidth() const;
	int getWindowHeight() const;

	float getWindowWidthFloat() const;
	float getWindowHeightFloat() const;

	float getWindowAspectRatio() const;

	const char* getWindowName() const;

private:
	UserConfig();

private:
	int   m_windowWidth;
	int   m_windowHeight;

	float m_windowWidthFloat;
	float m_windowHeightFloat;

	const char* m_windowName;
};
