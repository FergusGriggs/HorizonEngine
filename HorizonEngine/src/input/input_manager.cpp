#include "input_manager.h"

namespace hrzn
{
    InputManager::InputManager()
    {
    }

    InputManager::~InputManager()
    {
    }

    InputManager& InputManager::it()
    {
        static InputManager s;
        return s;
    }

	void InputManager::setMouseAndKeyboardPtrs(input::Mouse* mouse, input::Keyboard* keyboard)
	{
		m_mouse = mouse;
		m_keyboard = keyboard;
	}

    void InputManager::update(float deltaTime)
    {
		while (!m_keyboard->isCharBufferEmpty())
		{
			unsigned char ch = m_keyboard->readChar();
		}

		while (!m_keyboard->isKeyBufferEmpty())
		{
			input::KeyboardEvent keyboardEvent = m_keyboard->readKey();

			for (size_t delegateIndex = 0; delegateIndex < m_keyboardDelegates.size(); ++delegateIndex)
			{
				m_keyboardDelegates[delegateIndex](keyboardEvent, deltaTime);
			}
		}
		while (!m_mouse->isEventBufferEmpty())
		{
			input::MouseEvent mouseEvent = m_mouse->readEvent();

			for (size_t delegateIndex = 0; delegateIndex < m_mouseDelegates.size(); ++delegateIndex)
			{
				m_mouseDelegates[delegateIndex](mouseEvent, deltaTime);
			}
		}
    }

	void InputManager::registerMouseDelegate(std::function<void(const input::MouseEvent&, float)> del)
	{
		m_mouseDelegates.push_back(del);
	}

	void InputManager::registerKeyboardDelegate(std::function<void(const input::KeyboardEvent&, float)> del)
	{
		m_keyboardDelegates.push_back(del);
	}
}
