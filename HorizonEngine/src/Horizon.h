
//A singleton class that encapsulates all of the engine code

#pragma once

#include <Audio.h>

#include "window_container.h"
#include "utils/timer.h"
#include "entity/utils/game_object_controller.h"

namespace hrzn
{
	class Horizon : WindowContainer
	{
	public:
		Horizon();
		Horizon(const Horizon&);

		bool initialize(HINSTANCE hInstance, std::string windowClass);

		bool processMessages();
		void update();
		void renderFrame();

	private:
		utils::Timer m_timer;
		float        m_deltaTime;

		AudioEngine*         m_audioEngine;
		SoundEffect*         m_soundEffect;

		entity::ControllerManager m_controllerManager;
	};
}
