
//Function implementations for the Horizon class

#include "Horizon.h"

namespace hrzn
{
	Horizon::Horizon() :
		WindowContainer(),

		m_timer(),
		m_deltaTime(0.0f),

		m_audioEngine(nullptr),
		m_soundEffect(nullptr),

		m_controllerManager(&m_keyboard)
	{
		/*audioEngine = new AudioEngine();

		soundEffect = new SoundEffect(audioEngine, L"/res/audio/music.wav");
		auto effect = soundEffect->CreateInstance();
		effect->Play(true);*/
	}

	Horizon::Horizon(const Horizon&) :
		WindowContainer(),

		m_timer(),
		m_deltaTime(0.0f),
		
		m_audioEngine(nullptr),
		m_soundEffect(nullptr),
		
		m_controllerManager(&m_keyboard)
	{
	}

	bool Horizon::initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
	{
		m_timer.start();

		m_controllerManager.addController(&m_graphicsHandler.getCamera(), entity::ControllerType::eCamera, 0.005f);
		m_controllerManager.getControllers().at(0).setActive(true);

		if (!m_renderWindow.initialize(this, hInstance, windowTitle, windowClass, width, height))
		{
			return false;
		}

		if (!m_graphicsHandler.initialize(m_renderWindow.getHWND(), width, height, &m_controllerManager))
		{
			return false;
		}

		return true;
	}

	bool Horizon::processMessages()
	{
		return m_renderWindow.processMessages();
	}

	void Horizon::update()
	{
		m_deltaTime = m_timer.getDeltaTime();
		m_timer.restart();

		while (!m_keyboard.isCharBufferEmpty())
		{
			unsigned char ch = m_keyboard.readChar();
		}

		while (!m_keyboard.isKeyBufferEmpty())
		{
			input::KeyboardEvent keyboardEvent = m_keyboard.readKey();
			unsigned char keyCode = keyboardEvent.getKeyCode();
		}
		while (!m_mouse.isEventBufferEmpty())
		{
			input::MouseEvent mouseEvent = m_mouse.readEvent();
			if (mouseEvent.getType() == input::MouseEvent::EventType::eRawMove)
			{
				if (m_mouse.isRightDown())
				{
					float moveFactor = m_graphicsHandler.getCamera().getZoom() / 90.0f;
					if (moveFactor > 1.0f) moveFactor = 1.0f;
					m_graphicsHandler.getCamera().getTransform().rotateUsingAxis(m_graphicsHandler.getCamera().getTransform().getRightVector(), (float)mouseEvent.getPos().y * 0.01f * moveFactor);
					m_graphicsHandler.getCamera().getTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.getPos().x * 0.01f * moveFactor);
				}
			}

			if (mouseEvent.getType() == input::MouseEvent::EventType::eScrollUp)
			{
				if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
				{
					m_graphicsHandler.getCamera().zoom(-2.0f);
				}
			}

			if (mouseEvent.getType() == input::MouseEvent::EventType::eScrollDown)
			{
				if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
				{
					m_graphicsHandler.getCamera().zoom(2.0f);
				}
			}

			if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftPress)
			{
				if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
				{
					m_graphicsHandler.checkSelectingObject();
				}
			}
			if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftRelease)
			{
				if (!(m_graphicsHandler.getAxisEditSubState() == gfx::AxisEditSubState::eEditNone))
				{
					m_graphicsHandler.stopAxisEdit();
				}
			}
		}

		m_graphicsHandler.setMouseX(m_mouse.getPos().x);
		m_graphicsHandler.setMouseY(m_mouse.getPos().y);

		m_controllerManager.updateControllers(m_deltaTime);

		m_graphicsHandler.update(m_deltaTime);
	}

	void Horizon::renderFrame()
	{
		m_graphicsHandler.renderFrame(m_deltaTime);
	}
}