#include "Controller.h"

Controller::Controller(GameObject* gameObject, Keyboard* keyboard, ControllerType controllerType, float moveSpeed)
{
	this->gameObject = gameObject;
	this->keyboard = keyboard;
	this->controllerType = controllerType;
	this->moveSpeed = moveSpeed;
	this->active = false;
}

void Controller::UpdateObject(float deltaTime)
{
	float speedMod = 1.0f;

	switch (controllerType)
	{
	case ControllerType::CAMERA:
		if (keyboard->KeyIsPressed(VK_SHIFT))
		{
			speedMod = 3.0f;
		}

		if (keyboard->KeyIsPressed('W'))
		{
			gameObject->AdjustPosition(gameObject->GetFrontVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('S'))
		{
			gameObject->AdjustPosition(gameObject->GetBackVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('A')) {
			gameObject->AdjustPosition(gameObject->GetLeftVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('D')) {
			gameObject->AdjustPosition(gameObject->GetRightVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('E')) {
			gameObject->AdjustPosition(0.0f, moveSpeed * speedMod * deltaTime, 0.0f);
		}
		if (keyboard->KeyIsPressed('Q')) {
			gameObject->AdjustPosition(0.0f, -moveSpeed * speedMod * deltaTime, 0.0f);
		}
		break;
	case ControllerType::HORIZONTAL:
		if (keyboard->KeyIsPressed('W'))
		{
			gameObject->AdjustPosition(gameObject->GetFrontVector() * moveSpeed * deltaTime);
		}
		if (keyboard->KeyIsPressed('S'))
		{
			gameObject->AdjustPosition(gameObject->GetBackVector() * moveSpeed * deltaTime);
		}
		if (keyboard->KeyIsPressed('A')) {
			gameObject->RotateAxisVectors(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -0.01f);
			//gameObject->AdjustPosition(gameObject->GetLeftVector() * moveSpeed * deltaTime);
		}
		if (keyboard->KeyIsPressed('D')) {
			gameObject->RotateAxisVectors(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.01f);
			//gameObject->AdjustPosition(gameObject->GetRightVector() * moveSpeed * deltaTime);
		}
		break;
	}
}

bool Controller::IsActive()
{
	return active;
}

bool* Controller::IsActivePtr()
{
	return &active;
}

void Controller::SetActive(bool active)
{
	this->active = active;
}

GameObject* Controller::GetGameObject()
{
	return gameObject;
}
