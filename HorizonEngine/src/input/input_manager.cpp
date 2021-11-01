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
			
			switch (mouseEvent.getType())
			{
			case input::MouseEvent::EventType::eLeftPress:
			case input::MouseEvent::EventType::eLeftRelease:
			case input::MouseEvent::EventType::eRightPress:
			case input::MouseEvent::EventType::eRightRelease:
			case input::MouseEvent::EventType::eMiddlePress:
			case input::MouseEvent::EventType::eMiddleRelease:
				for (size_t delegateIndex = 0; delegateIndex < m_mouseButtonDelegates.size(); ++delegateIndex)
				{
					m_mouseButtonDelegates[delegateIndex](mouseEvent, deltaTime);
				}
				break;
			case input::MouseEvent::EventType::eScrollUp:
			case input::MouseEvent::EventType::eScrollDown:
				for (size_t delegateIndex = 0; delegateIndex < m_mouseScrollDelegates.size(); ++delegateIndex)
				{
					m_mouseScrollDelegates[delegateIndex](mouseEvent, deltaTime);
				}
				break;
			case input::MouseEvent::EventType::eMove:
			case input::MouseEvent::EventType::eRawMove:
				for (size_t delegateIndex = 0; delegateIndex < m_mouseMoveDelegates.size(); ++delegateIndex)
				{
					m_mouseMoveDelegates[delegateIndex](mouseEvent, deltaTime);
				}
				break;
			case input::MouseEvent::EventType::eInvalid:
				break;
			}
		}
    }
	
	void InputManager::registerMouseMoveDelegate(std::function<void(const input::MouseEvent&, float)> del)
	{
		m_mouseMoveDelegates.push_back(del);
	}

	void InputManager::registerMouseButtonDelegate(std::function<void(const input::MouseEvent&, float)> del)
	{
		m_mouseButtonDelegates.push_back(del);
	}

	void InputManager::registerMouseScrollDelegate(std::function<void(const input::MouseEvent&, float)> del)
	{
		m_mouseScrollDelegates.push_back(del);
	}

	void InputManager::registerKeyboardDelegate(std::function<void(const input::KeyboardEvent&, float)> del)
	{
		m_keyboardDelegates.push_back(del);
	}

	const input::MousePosPixel& InputManager::getMousePos() const
	{
		return m_mouse->getPos();
	}

	const input::MousePosNDC& InputManager::getMousePosNDC() const
	{
		return m_mouse->getPosNDC();
	}

	bool InputManager::isLeftMouseDown() const
	{
		return m_mouse->isLeftDown();
	}

	bool InputManager::isRightMouseDown() const
	{
		return m_mouse->isRightDown();
	}

	bool InputManager::isMiddleMouseDown() const
	{
		return m_mouse->isMiddleDown();
	}

	bool InputManager::isKeyPressed(unsigned char keyCode) const
	{
		return m_keyboard->isKeyPressed(keyCode);
	}
}
