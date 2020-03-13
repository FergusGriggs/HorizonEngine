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

	controllerManager = new ControllerManager(&keyboard);
	controllerManager->AddController(&this->graphics.camera, ControllerType::CAMERA, 0.005f);
	controllerManager->GetControllers()->at(0).SetActive(true);

	if (!this->renderWindow.Initialize(this, hInstance, windowTitle, windowClass, width, height))
	{
		return false;
	}
	if (!this->graphics.Initialize(this->renderWindow.GetHWND(), width, height, controllerManager))
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
	deltaTime = timer.GetMillisecondsElapsed() * 0.001f;
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
				this->graphics.camera.GetTransform()->RotateUsingAxis(this->graphics.camera.GetTransform()->GetRightVector(), (float)mouseEvent.GetPosY() * 0.01f * moveFactor);
				this->graphics.camera.GetTransform()->RotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.GetPosX() * 0.01f * moveFactor);
			}
		}
		
		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_UP)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				this->graphics.camera.Zoom(-2.0f);
			}
		}

		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_DOWN)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				this->graphics.camera.Zoom(2.0f);
			}
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

	controllerManager->UpdateControllers(deltaTime);
	
	this->graphics.Update(deltaTime);
}

void Horizon::RenderFrame() {
	graphics.RenderFrame(deltaTime);
}