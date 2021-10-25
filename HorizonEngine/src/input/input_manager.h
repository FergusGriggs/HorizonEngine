#pragma once

#include "keyboard/keyboard.h"
#include "mouse/mouse.h"

#include <functional>

namespace hrzn
{
    class InputManager
    {
    public:
        InputManager();
        ~InputManager();

        InputManager& it();

        void setMouseAndKeyboardPtrs(input::Mouse* mouse, input::Keyboard* keyboard);

        void update(float deltaTime);
        
        void registerMouseDelegate(std::function<void(const input::MouseEvent&, float)> del);
        void registerKeyboardDelegate(std::function<void(const input::KeyboardEvent&, float)> del);

    private:
        input::Mouse*    m_mouse;
        input::Keyboard* m_keyboard;

        std::vector<std::function<void(const input::MouseEvent&, float)>>    m_mouseDelegates;
        std::vector<std::function<void(const input::KeyboardEvent&, float)>> m_keyboardDelegates;
    };
}

