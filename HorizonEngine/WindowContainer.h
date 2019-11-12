#pragma once

#include "RenderWindow.h"
#include "keyboard/Keyboard.h"
#include "mouse/Mouse.h"

#include "graphics/Graphics.h"

#include <memory>

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow renderWindow;
	Keyboard keyboard;
	Mouse mouse;
	Graphics graphics;
private:

};

