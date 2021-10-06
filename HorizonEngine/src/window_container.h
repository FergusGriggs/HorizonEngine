#pragma once

#include <memory>

#include "render_window.h"
#include "graphics/graphics_handler.h"

#include "input/keyboard/keyboard.h"
#include "input/mouse/mouse.h"

namespace hrzn
{
	class WindowContainer
	{
	public:
		WindowContainer();

		LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		RenderWindow         m_renderWindow;
		gfx::GraphicsHandler m_graphicsHandler;

		input::Keyboard      m_keyboard;
		input::Mouse         m_mouse;
	};
}
