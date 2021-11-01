#pragma once

#include "keyboard/keyboard.h"
#include "mouse/mouse.h"

#include <functional>

namespace hrzn
{
    class InputManager
    {
    public:
        ~InputManager();

        static InputManager& it();

        void setMouseAndKeyboardPtrs(input::Mouse* mouse, input::Keyboard* keyboard);

        void update(float deltaTime);
        
        void registerMouseMoveDelegate(std::function<void(const input::MouseEvent&, float)> del);
        void registerMouseButtonDelegate(std::function<void(const input::MouseEvent&, float)> del);
        void registerMouseScrollDelegate(std::function<void(const input::MouseEvent&, float)> del);

        void registerKeyboardDelegate(std::function<void(const input::KeyboardEvent&, float)> del);

        const input::MousePosPixel& getMousePos() const;
        const input::MousePosNDC&   getMousePosNDC() const;

        bool isLeftMouseDown() const;
        bool isRightMouseDown() const;
        bool isMiddleMouseDown() const;

        bool isKeyPressed(unsigned char kekeyCodey) const;

    private:
        InputManager();

    private:
        input::Mouse*    m_mouse;
        input::Keyboard* m_keyboard;

        std::vector<std::function<void(const input::MouseEvent&, float)>>    m_mouseMoveDelegates;
        std::vector<std::function<void(const input::MouseEvent&, float)>>    m_mouseButtonDelegates;
        std::vector<std::function<void(const input::MouseEvent&, float)>>    m_mouseScrollDelegates;

        std::vector<std::function<void(const input::KeyboardEvent&, float)>> m_keyboardDelegates;
    };
}

