//HorizonEngine.cpp
//Function implementations for the HorizonEngine class

#include "Horizon.h"

Horizon::Horizon() {
	/*audioEngine = new AudioEngine();

	soundEffect = new SoundEffect(audioEngine, L"/res/audio/music.wav");
	auto effect = soundEffect->CreateInstance();
	effect->Play(true);*/
}

Horizon::Horizon(const Horizon&) {

}

bool Horizon::Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height) {

	timer.Start();

	if (!this->renderWindow.Initialize(this, hInstance, windowTitle, windowClass, width, height)) {
		return false;
	}
	if (!graphics.Initialize(this->renderWindow.GetHWND(), width, height)) {
		return false;
	}

	return true;
}

bool Horizon::ProcessMessages() {
	return this->renderWindow.ProcessMessages();
}

void Horizon::Update() {
	deltaTime = timer.GetMillisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty()) {
		unsigned char ch = keyboard.ReadChar();
	}
	while (!keyboard.KeyBufferIsEmpty()) {
		KeyboardEvent keyboardEvent = keyboard.ReadKey();
		unsigned char keyCode = keyboardEvent.GetKeyCode();
	}
	while (!mouse.EventBufferIsEmpty()) {
		MouseEvent mouseEvent = mouse.ReadEvent();
		if (mouseEvent.GetType() == MouseEvent::EventType::RAW_MOVE) {
			if (mouse.IsRightDown()) {
				float moveFactor = this->graphics.camera.GetZoom() / 90.0f;
				if (moveFactor > 1.0f) moveFactor = 1.0f;
				this->graphics.camera.RotateAxisVectors(this->graphics.camera.GetRightVector(), -(float)mouseEvent.GetPosY() * 0.01f * moveFactor);
				this->graphics.camera.RotateAxisVectors(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.GetPosX() * 0.01f * moveFactor);
				//this->graphics.camera.AdjustRotation((float)mouseEvent.GetPosY() * 0.01f * moveFactor, (float)mouseEvent.GetPosX() * 0.01f * moveFactor, 0.0f);
			}
		}
		
		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_UP) {
			this->graphics.camera.Zoom(-2.0f);
		}

		if (mouseEvent.GetType() == MouseEvent::EventType::SCROLL_DOWN) {
			this->graphics.camera.Zoom(2.0f);
		}

		if (mouseEvent.GetType() == MouseEvent::EventType::LEFT_PRESS) {
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())) {
				this->graphics.CheckSelectingObject();
			}
		}
		if (mouseEvent.GetType() == MouseEvent::EventType::LEFT_RELEASE) {
			if (!(this->graphics.GetAxisEditSubState() == AxisEditSubState::EDIT_NONE)) {
				this->graphics.StopAxisEdit();
			}
		}
	}

	this->graphics.SetMouseX(mouse.GetPosX());
	this->graphics.SetMouseY(mouse.GetPosY());

	float cameraSpeed = 0.005f;
	
	if (keyboard.KeyIsPressed(VK_SHIFT)) {
		cameraSpeed *= 3.0f;
	}

	if (keyboard.KeyIsPressed('W')) {
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetFrontVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('S')) {
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetBackVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('A')) {
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetRightVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('D')) {
		this->graphics.camera.AdjustPosition(this->graphics.camera.GetLeftVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('E')) {
		this->graphics.camera.AdjustPosition(0.0f, cameraSpeed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('Q')) {
		this->graphics.camera.AdjustPosition(0.0f, -cameraSpeed * deltaTime, 0.0f);
	}

	this->graphics.Update(deltaTime);
}

void Horizon::RenderFrame() {
	graphics.RenderFrame(deltaTime);
}