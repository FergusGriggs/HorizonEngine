//HorizonEngine.h
//A singleton class that encapsulates all of the engine code

#pragma once

#include <Audio.h>

#include "WindowContainer.h"
#include "utility/Timer.h"
#include "graphics/utility/Controller.h"

class Horizon : WindowContainer
{
public:
	Horizon();
	Horizon(const Horizon&);
	bool Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
	bool InitializeControllers();

private:
	Timer timer;
	float deltaTime;
	AudioEngine* audioEngine;
	SoundEffect* soundEffect;

	std::vector<Controller> controllers;
};

