
//Function implementations for the Horizon class

#include "horizon.h"

#include "user_config.h"
#include "input/input_manager.h"

namespace hrzn
{
	Horizon::Horizon() :
		WindowContainer(),

		m_timer(),
		m_deltaTime(0.0f),

		m_audioEngine(nullptr),
		m_soundEffect(nullptr),

		m_graphicsHandler(),
		m_controllerManager(),
		m_sceneManager(&m_controllerManager)
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
		
		m_graphicsHandler(),
		m_controllerManager(),
		m_sceneManager(&m_controllerManager)
	{
	}

	bool Horizon::initialize(HINSTANCE hInstance, std::string windowClass)
	{
		m_timer.start();

		if (!m_renderWindow.initialize(this, hInstance, UserConfig::it().getWindowName(), windowClass, UserConfig::it().getWindowWidth(), UserConfig::it().getWindowHeight()))
		{
			return false;
		}

		if (!m_graphicsHandler.initialize(m_renderWindow.getHWND()))
		{
			return false;
		}

		m_sceneManager.initialise();

		if (!m_graphicsHandler.initializeScene(m_sceneManager))
		{
			return false;
		}

		m_controllerManager.addController(&m_sceneManager.getWritableActiveCamera(), entity::ControllerType::eCamera, 5.0f, true);

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

		InputManager::it().update(m_deltaTime);

		m_controllerManager.updateControllers(m_deltaTime);

		m_sceneManager.update(m_deltaTime);
		m_graphicsHandler.update(m_sceneManager, m_deltaTime);
	}

	void Horizon::renderFrame()
	{
		m_graphicsHandler.renderActiveScene(m_sceneManager);
	}
}