#include "WindowContainer.h"

WindowContainer::WindowContainer() {
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false) {
		RAWINPUTDEVICE rawInputDevice;

		rawInputDevice.usUsagePage = 0x01;
		rawInputDevice.usUsage = 0x02;
		rawInputDevice.dwFlags = 0;
		rawInputDevice.hwndTarget = 0;

		if (RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)) == FALSE) {
			ErrorLogger::Log(GetLastError(), "Failed to register raw input devices.");
			exit(-1);
		}

		raw_input_initialized = true;
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		return true;
	}

	switch (uMsg) {
		//keyboard messages
	case WM_CHAR:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.IsCharsAutoRepeat()) {
			keyboard.OnChar(ch);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				keyboard.OnChar(ch);
			}
		}
		return 0;
	}
	case WM_KEYDOWN:
	{
		unsigned char keyCode = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat()) {
			keyboard.OnKeyPressed(keyCode);
		}
		else {
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed) {
				keyboard.OnKeyPressed(keyCode);
			}
		}
		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char keyCode = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(keyCode);
		return 0;
	}
	//mouse messages
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMouseMove(x, y);
		return 0;
	}
	case WM_INPUT:
	{
		UINT dataSize;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		
		if (dataSize > 0) {
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize) {
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE) {
					mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			mouse.OnScrollUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
			mouse.OnScrollDown(x, y);
		}
		return 0;
	}
	default:
		DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
}