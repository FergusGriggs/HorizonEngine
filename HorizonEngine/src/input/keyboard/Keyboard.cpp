
//Function implementations for the Keyboard class

#include "keyboard.h"

namespace hrzn::input
{
	Keyboard::Keyboard() :
		m_keyStates(),

		m_keyBuffer(),
		m_charBuffer(),

		m_autoRepeatKeys(false),
		m_autoRepeatChars(false)
	{
		//Initialize key states
		for (int i = 0; i < 256; i++)
		{
			m_keyStates[i] = false;
		}
	}

	bool Keyboard::isKeyPressed(const unsigned char keyCode) const
	{
		return m_keyStates[keyCode];
	}

	bool Keyboard::isKeyBufferEmpty() const
	{
		return m_keyBuffer.empty();
	}

	bool Keyboard::isCharBufferEmpty() const
	{
		return m_charBuffer.empty();
	}

	KeyboardEvent Keyboard::readKey()
	{
		if (m_keyBuffer.empty())
		{
			return KeyboardEvent();
		}
		else
		{
			KeyboardEvent keyboardEvent = m_keyBuffer.front();
			m_keyBuffer.pop();
			return keyboardEvent;
		}
	}

	unsigned char Keyboard::readChar()
	{
		if (m_charBuffer.empty())
		{
			return 0u;
		}
		else
		{
			unsigned char charEvent = m_charBuffer.front();
			m_charBuffer.pop();
			return charEvent;
		}
	}

	void Keyboard::onKeyPressed(const unsigned char key)
	{
		m_keyStates[key] = true;
		m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::ePress, key));
	}

	void Keyboard::onKeyReleased(const unsigned char key)
	{
		m_keyStates[key] = false;
		m_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::eRelease, key));
	}

	void Keyboard::onChar(const unsigned char key)
	{
		m_charBuffer.push(key);
	}

	void Keyboard::setAutoRepeatKeys(bool autoRepeat)
	{
		m_autoRepeatKeys = autoRepeat;
	}

	void Keyboard::setAutoRepeatChars(bool autoRepeat)
	{
		m_autoRepeatChars = autoRepeat;
	}

	bool Keyboard::isAutoRepeatKeysOn() const
	{
		return m_autoRepeatKeys;
	}

	bool Keyboard::isAutoRepeatCharsOn() const
	{
		return m_autoRepeatChars;
	}
}
