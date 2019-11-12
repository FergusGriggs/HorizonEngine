#include "WindowContainer.h"

bool RenderWindow::Initialize(WindowContainer* windowContainer, HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {
	this->hInstance = hInstance;
	this->windowTitle = windowTitle;
	this->windowTitleWide = StringHelper::StringToWide(this->windowTitle);
	this->windowClass = windowClass;
	this->windowClassWide = StringHelper::StringToWide(this->windowClass);
	this->width = width;
	this->height = height;

	this->RegisterWindowClass();

	int screenCentreX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	int screenCentreY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;

	RECT windowRect;

	windowRect.left = screenCentreX;
	windowRect.top = screenCentreY;
	windowRect.right = windowRect.left + this->width;
	windowRect.bottom = windowRect.top + this->height;

	AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	this->handle = CreateWindowEx(0, // default style
		this->windowClassWide.c_str(),
		this->windowTitleWide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // window style
		windowRect.left,// x pos
		windowRect.top,// y pos
		windowRect.right - windowRect.left, // width
		windowRect.bottom - windowRect.top, // height
		NULL, // parent window non existent
		NULL, // child window non existent
		this->hInstance, // module instance for window
		windowContainer);

	if (this->handle == NULL) {
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for: " + this->windowTitle);
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool RenderWindow::ProcessMessages() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // init message structure

	while (PeekMessage(&msg, this->handle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL) {
		if (!IsWindow(this->handle)) {
			this->handle = NULL;
			UnregisterClass(this->windowClassWide.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const {
	return this->handle;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);

		if (pWindow == nullptr) {
			ErrorLogger::Log("Critical Error: Pointer to window container was null during WM_NCCREATE.");
			exit(-1);
		}

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX windowClass;
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = HandleMessageSetup;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = this->hInstance;
	windowClass.hIcon = NULL;  // no icon
	windowClass.hIconSm = NULL; // no small icon
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // default cursor
	windowClass.hbrBackground = NULL; // no background brush
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = this->windowClassWide.c_str();
	windowClass.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&windowClass);
}

RenderWindow::~RenderWindow() {
	if (this->handle != NULL) {
		UnregisterClass(this->windowClassWide.c_str(), this->hInstance);
		DestroyWindow(this->handle);
	}
}