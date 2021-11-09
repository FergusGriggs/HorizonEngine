
//Manages KeyboardEvent stack, populated using windows events. 

#pragma once

#include "keyboard_event.h"

#include <queue>

namespace hrzn::input
{
	class Keyboard
	{
	public:
		Keyboard();

		bool isKeyPressed(const unsigned char keyCode) const;
		bool isKeyBufferEmpty() const;
		bool isCharBufferEmpty() const;

		KeyboardEvent readKey();
		unsigned char readChar();

		void onKeyPressed(const unsigned char key);
		void onKeyReleased(const unsigned char key);
		void onChar(const unsigned char key);

		void setAutoRepeatKeys(bool autoRepeat);
		void setAutoRepeatChars(bool autoRepeat);

		bool isAutoRepeatKeysOn() const;
		bool isAutoRepeatCharsOn() const;

	private:
		bool                      m_keyStates[256];

		std::queue<KeyboardEvent> m_keyBuffer;
		std::queue<unsigned char> m_charBuffer;

		bool                      m_autoRepeatKeys;
		bool                      m_autoRepeatChars;
	};
}
