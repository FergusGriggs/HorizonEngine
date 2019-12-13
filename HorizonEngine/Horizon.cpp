//HorizonEngine.cpp
//Function implementations for the Horizon class

#include "Horizon.h"

Horizon::Horizon() {
	/*audioEngine = new AudioEngine();

	soundEffect = new SoundEffect(audioEngine, L"/res/audio/music.wav");
	auto effect = soundEffect->CreateInstance();
	effect->Play(true);*/
}

Horizon::Horizon(const Horizon&) {

}

bool Horizon::Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height)
{

	timer.Start();

	if (!this->renderWindow.Initialize(this, hInstance, windowTitle, windowClass, width, height))
	{
		return false;
	}
	if (!this->graphics.Initialize(this->renderWindow.GetHWND(), width, height, &controllers))
	{
		return false;
	}
	if (!this->InitializeControllers())
	{
		return false;
	}

	return true;
}

bool Horizon::ProcessMessages()
{
	return this->renderWindow.ProcessMessages();
}

void Horizon::Update()
{
	deltaTime = timer.GetMillisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent keyboardEvent = keyboard.ReadKey();
		unsigned char keyCode = keyboardEvent.GetKeyCode();
	}
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent mouseEvent = mouse.ReadEvent();
		if (mouseEvent.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			if (mouse.IsRightDown())
			{
				float moveFactor = this->graphics.camera.GetZoom() / 90.0f;
				if (moveFactor > 1.0f) moveFactor = 1.0f;
				this->graphics.camera.RotateAxisVectors(this->graphics.camera.GetRightVector(), -(float)mouseEvent.GetPosY() * 0.01f * moveFactor);
				this->graphics.camera.RotateAxisVectors(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.GetPosX() * 0.01f * moveFactor);
				//this->graphics.camera.AdjustRotation((float)mouseEvent.GetPosY() * 0.01f * moveFactor, (float)mouseEvent.GetPosX() * 0.01f * moveFactor, 0.0f);
			}
		}
		
		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_UP)
		{
			this->graphics.camera.Zoom(-2.0f);
		}

		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_DOWN)
		{
			this->graphics.camera.Zoom(2.0f);
		}

		if (mouseEvent.GetType() == MouseEvent::EventType::LEFT_PRESS)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				this->graphics.CheckSelectingObject();
			}
		}
		if (mouseEvent.GetType() == MouseEvent::EventType::LEFT_RELEASE)
		{
			if (!(this->graphics.GetAxisEditSubState() == AxisEditSubState::EDIT_NONE))
			{
				this->graphics.StopAxisEdit();
			}
		}
	}

	this->graphics.SetMouseX(mouse.GetPosX());
	this->graphics.SetMouseY(mouse.GetPosY());

	size_t controllerSize = controllers.size();
	for (size_t i = 0; i < controllerSize; ++i)
	{
		if (controllers.at(i).IsActive())
		{
			controllers.at(i).UpdateObject(deltaTime);
		}
	}
	

	this->graphics.Update(deltaTime);
}

void Horizon::RenderFrame() {
	graphics.RenderFrame(deltaTime);
}

bool Horizon::InitializeControllers()
{
	controllers.push_back(Controller(&this->graphics.camera, &keyboard, ControllerType::CAMERA, 0.005f));
	controllers.push_back(Controller(this->graphics.GetGameObject("Boat"), &keyboard, ControllerType::HORIZONTAL, 0.01f));
	controllers.push_back(Controller(this->graphics.GetGameObject("Man"), &keyboard, ControllerType::HORIZONTAL, 0.01f));
	controllers.push_back(Controller(this->graphics.GetGameObject("Lady"), &keyboard, ControllerType::HORIZONTAL, 0.01f));

	controllers.at(0).SetActive(true);

	return true;
}
