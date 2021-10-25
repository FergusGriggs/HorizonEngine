
//Function implementations for the Horizon class

#include "horizon.h"

#include "user_config.h"

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
		/*m_audioEngine = new AudioEngine(AudioEngine_Default);

		m_soundEffect = new SoundEffect(m_audioEngine, L"/res/audio/music.wav");
		auto effect = m_soundEffect->CreateInstance();
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

	bool Horizon::initialize(HINSTANCE hInstance, std::string windowClass)
	{
		m_timer.start();

		m_controllerManager.addController(&m_graphicsHandler.getCamera(), entity::ControllerType::eCamera, 0.005f);
		m_controllerManager.getControllers().at(0).setActive(true);

		if (!m_renderWindow.initialize(this, hInstance, UserConfig::it().getWindowName(), windowClass, UserConfig::it().getWindowWidth(), UserConfig::it().getWindowHeight()))
		{
			return false;
		}

		if (!m_graphicsHandler.initialize(m_renderWindow.getHWND(), &m_controllerManager))
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

		m_graphicsHandler.setMouseX(m_mouse.getPos().x);
		m_graphicsHandler.setMouseY(m_mouse.getPos().y);

		m_controllerManager.updateControllers(m_deltaTime);

		m_graphicsHandler.update(m_deltaTime);
	}

	void Horizon::renderFrame()
	{
		m_graphicsHandler.renderActiveScene(m_sceneManager);
	}
}