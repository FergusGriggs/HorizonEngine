
//Function implementations for the RenderWindow class

#include "window_container.h"

namespace hrzn
{
	RenderWindow::RenderWindow()
	{
	}

	RenderWindow::~RenderWindow()
	{
		if (m_handle != NULL)
		{
			UnregisterClass(m_windowClassWide.c_str(), m_hInstance);
			DestroyWindow(m_handle);
		}
	}

	bool RenderWindow::initialize(WindowContainer* windowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
	{
		m_hInstance = hInstance;
		m_windowTitle = windowTitle;
		m_windowTitleWide = utils::string_helpers::stringToWide(windowTitle);
		m_windowClass = windowClass;
		m_windowClassWide = utils::string_helpers::stringToWide(windowClass);
		m_width = width;
		m_height = height;

		registerWindowClass();

		int screenCentreX = GetSystemMetrics(SM_CXSCREEN) / 2 - m_width / 2;
		int screenCentreY = GetSystemMetrics(SM_CYSCREEN) / 2 - m_height / 2;

		RECT windowRect;

		windowRect.left = screenCentreX;
		windowRect.top = screenCentreY;
		windowRect.right = windowRect.left + width;
		windowRect.bottom = windowRect.top + height;

		AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		m_handle = CreateWindowEx(0, // default style
			m_windowClassWide.c_str(),
			m_windowTitleWide.c_str(),
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // window style
			windowRect.left,// x pos
			windowRect.top,// y pos
			windowRect.right - windowRect.left, // width
			windowRect.bottom - windowRect.top, // height
			NULL, // parent window non existent
			NULL, // child window non existent
			m_hInstance, // module instance for window
			windowContainer);

		if (m_handle == NULL)
		{
			utils::ErrorLogger::log(GetLastError(), "CreateWindowEX Failed for: " + windowTitle);
			return false;
		}

		ShowWindow(m_handle, SW_SHOW);
		SetForegroundWindow(m_handle);
		SetFocus(m_handle);

		return true;
	}

	bool RenderWindow::processMessages()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG)); // init message structure

		while (PeekMessage(&msg, m_handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_NULL)
		{
			if (!IsWindow(m_handle))
			{
				m_handle = NULL;
				UnregisterClass(m_windowClassWide.c_str(), m_hInstance);
				return false;
			}
		}

		return true;
	}

	HWND RenderWindow::getHWND() const
	{
		return m_handle;
	}

	LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

		default:
			WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

			return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);

			if (pWindow == nullptr)
			{
				utils::ErrorLogger::log("Critical Error: Pointer to window container was null during WM_NCCREATE.");
				exit(-1);
			}

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
			return pWindow->windowProc(hwnd, uMsg, wParam, lParam);
		}
		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		}
	}

	void RenderWindow::registerWindowClass()
	{
		WNDCLASSEX windowClass;
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = HandleMessageSetup;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_hInstance;
		windowClass.hIcon = NULL;  // no icon
		windowClass.hIconSm = NULL; // no small icon
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // default cursor
		windowClass.hbrBackground = NULL; // no background brush
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = m_windowClassWide.c_str();
		windowClass.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&windowClass);
	}
}
