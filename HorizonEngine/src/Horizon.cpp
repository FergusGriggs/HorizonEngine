
//Function implementations for the Horizon class

#include "horizon.h"

#include "graphics/graphics_handler.h"

#include "scene/scene_manager.h"

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

		m_controllerManager()
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
		
		m_controllerManager()
	{
	}

	bool Horizon::initialize(HINSTANCE hInstance, std::string windowClass)
	{
		m_timer.start();

		if (!m_renderWindow.initialize(this, hInstance, UserConfig::it().getWindowName(), windowClass, UserConfig::it().getWindowWidth(), UserConfig::it().getWindowHeight()))
		{
			return false;
		}

		if (!gfx::GraphicsHandler::it().initialize(m_renderWindow.getHWND()))
		{
			return false;
		}

		scene::SceneManager::it().initialise(&m_controllerManager);

		if (!gfx::GraphicsHandler::it().initializeScene())
		{
			return false;
		}

		
		m_controllerManager.addController(&scene::SceneManager::it().getWritableGameObject("$free_cam"), entity::ControllerType::eCamera, 5.0f, true);

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

		scene::SceneManager::it().update(m_deltaTime);
		gfx::GraphicsHandler::it().update(m_deltaTime);
	}

	void Horizon::renderFrame()
	{
		gfx::GraphicsHandler::it().render();
	}
}