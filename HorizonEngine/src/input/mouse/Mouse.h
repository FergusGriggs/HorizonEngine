#pragma once

#include "mouse_event.h"

#include <queue>

namespace hrzn::input
{
	class Mouse
	{
	public:
		Mouse();

		void onLeftPressed(int x, int y);
		void onLeftReleased(int x, int y);

		void onRightPressed(int x, int y);
		void onRightReleased(int x, int y);

		void onMiddlePressed(int x, int y);
		void onMiddleReleased(int x, int y);

		void onScrollUp(int x, int y);
		void onScrollDown(int x, int y);

		void onMouseMove(int x, int y);
		void onMouseMoveRaw(int x, int y);

		bool isLeftDown() const;
		bool isRightDown() const;
		bool isMiddleDown() const;

		void updateNDCPos();

		const MousePosPixel& getPos() const;
		const MousePosNDC&   getPosNDC() const;

		bool isEventBufferEmpty() const;
		MouseEvent readEvent();

	private:
		std::queue<MouseEvent> m_eventBuffer;
		MousePosPixel          m_pos;
		MousePosNDC            m_posNDC;

		bool m_leftIsDown;
		bool m_rightIsDown;
		bool m_middleIsDown;
	};
}
