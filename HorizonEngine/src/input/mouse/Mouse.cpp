#include "Mouse.h"
#include "mouse.h"

namespace hrzn::input
{
	Mouse::Mouse() :
		m_eventBuffer(),
		m_pos({ 0, 0 }),

		m_leftIsDown(false),
		m_rightIsDown(false),
		m_middleIsDown(false)
	{
	}

	void Mouse::onLeftPressed(int x, int y)
	{
		m_leftIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eLeftPress, x, y));
	}

	void Mouse::onLeftReleased(int x, int y)
	{
		m_leftIsDown = false;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eLeftRelease, x, y));
	}

	void Mouse::onRightPressed(int x, int y)
	{
		m_rightIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eRightPress, x, y));
	}

	void Mouse::onRightReleased(int x, int y)
	{
		m_rightIsDown = false;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eRightRelease, x, y));
	}

	void Mouse::onMiddlePressed(int x, int y)
	{
		m_middleIsDown = true;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eMiddlePress, x, y));
	}

	void Mouse::onMiddleReleased(int x, int y)
	{
		m_middleIsDown = false;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eMiddleRelease, x, y));
	}

	void Mouse::onScrollUp(int x, int y)
	{
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eScrollUp, x, y));
	}

	void Mouse::onScrollDown(int x, int y)
	{
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eScrollDown, x, y));
	}

	void Mouse::onMouseMove(int x, int y)
	{
		m_pos.x = x;
		m_pos.y = y;
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eMove, x, y));
	}

	void Mouse::onMouseMoveRaw(int x, int y)
	{
		m_eventBuffer.push(MouseEvent(MouseEvent::EventType::eRawMove, x, y));
	}

	bool Mouse::isLeftDown() const
	{
		return m_leftIsDown;
	}

	bool Mouse::isRightDown() const
	{
		return m_rightIsDown;
	}

	bool Mouse::isMiddleDown() const
	{
		return m_middleIsDown;
	}

	const MousePosPixel& Mouse::getPos() const
	{
		return m_pos;
	}

	const MousePosNDC& Mouse::getPosNDC() const
	{
		return m_posNDC;
	}

	bool Mouse::isEventBufferEmpty() const
	{
		return m_eventBuffer.empty();
	}

	MouseEvent Mouse::readEvent()
	{
		if (m_eventBuffer.empty())
		{
			return MouseEvent();
		}
		else
		{
			MouseEvent mouseEvent = m_eventBuffer.front();
			m_eventBuffer.pop();
			return mouseEvent;
		}
	}
}
