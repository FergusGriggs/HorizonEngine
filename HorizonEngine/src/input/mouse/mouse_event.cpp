#include "mouse_event.h"

namespace hrzn::input
{
	MouseEvent::MouseEvent() :
		m_type(MouseEvent::EventType::eInvalid),
		m_pos({ 0, 0 })
	{
	}

	MouseEvent::MouseEvent(const MouseEvent::EventType type, const int x, const int y) :
		m_type(type),
		m_pos({ x, y })
	{
	}

	bool MouseEvent::isValid() const
	{
		return m_type != EventType::eInvalid;
	}

	MouseEvent::EventType MouseEvent::getType() const
	{
		return m_type;
	}

	const MousePosPixel& MouseEvent::getPos() const
	{
		return m_pos;
	}
}
