#pragma once

namespace hrzn::input
{
	class KeyboardEvent
	{
	public:
		enum class EventType {
			ePress,
			eRelease,
			eInvalid
		};

		KeyboardEvent();
		KeyboardEvent(const EventType type, const unsigned char key);

		bool isPressed() const;
		bool isReleased() const;
		bool isValid() const;
		unsigned char getKeyCode() const;

	private:
		EventType     m_type;
		unsigned char m_key;
	};
}
