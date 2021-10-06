#include "keyboard_event.h"

namespace hrzn::input
{
	KeyboardEvent::KeyboardEvent() :
		m_type(EventType::eInvalid),
		m_key(0u)
	{
	}

	KeyboardEvent::KeyboardEvent(const EventType type, const unsigned char key) :
		m_type(type),
		m_key(key)
	{
	}

	bool KeyboardEvent::isPressed() const
	{
		return m_type == EventType::ePress;
	}

	bool KeyboardEvent::isReleased() const
	{
		return m_type == EventType::eRelease;
	}

	bool KeyboardEvent::isValid() const
	{
		return m_type != EventType::eInvalid;
	}

	unsigned char KeyboardEvent::getKeyCode() const
	{
		return m_key;
	}
}
