
//Manages window variables, including directx window handle

#pragma once

namespace hrzn
{
	class WindowContainer;
}

#include "utils/error_logger.h"

namespace hrzn
{
	class RenderWindow
	{
	public:
		RenderWindow();
		~RenderWindow();

		bool initialize(WindowContainer* windowContainer, HINSTANCE hInstance, const char* windowTitle, std::string windowClass, int width, int height);
		bool processMessages();
		HWND getHWND() const;
		
	private:
		void registerWindowClass();

	private:
		HWND         m_handle = NULL;// window handle
		HINSTANCE    m_hInstance = NULL;// instance handle

		const char*  m_windowTitle = "";
		std::wstring m_windowTitleWide = L"";
		std::string  m_windowClass = "";
		std::wstring m_windowClassWide = L"";

		int          m_width = 0;
		int          m_height = 0;
	};
}
