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
	deltaTime *= 1000.0;

	float speedMod = 1.0f;

	if (keyboard->KeyIsPressed(VK_SHIFT))
	{
		speedMod = 3.0f;
	}

	switch (controllerType)
	{
	case ControllerType::CAMERA:
		if (keyboard->KeyIsPressed('W'))
		{
			//gameObject->GetTransform()
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetFrontVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('S'))
		{
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetBackVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('A')) {
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetLeftVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('D')) {
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetRightVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('E')) {
			gameObject->GetTransform()->AdjustPosition(0.0f, moveSpeed * speedMod * deltaTime, 0.0f);
		}
		if (keyboard->KeyIsPressed('Q')) {
			gameObject->GetTransform()->AdjustPosition(0.0f, -moveSpeed * speedMod * deltaTime, 0.0f);
		}
		break;
	case ControllerType::HORIZONTAL:
		if (keyboard->KeyIsPressed('W'))
		{
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetFrontVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('S'))
		{
			gameObject->GetTransform()->AdjustPosition(gameObject->GetTransform()->GetBackVector() * moveSpeed * speedMod * deltaTime);
		}
		if (keyboard->KeyIsPressed('A')) {
			gameObject->GetTransform()->RotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -0.3f * moveSpeed * speedMod * deltaTime);
			//gameObject->AdjustPosition(gameObject->GetLeftVector() * moveSpeed * deltaTime);
		}
		if (keyboard->KeyIsPressed('D')) {
			gameObject->GetTransform()->RotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.3f * moveSpeed * speedMod * deltaTime);
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

float Controller::GetMoveSpeed()
{
	return this->moveSpeed;
}

ControllerType Controller::GetType()
{
	return this->controllerType;
}

GameObject* Controller::GetGameObject()
{
	return gameObject;
}

ControllerManager::ControllerManager(Keyboard* keyboard)
{
	this->keyboard = keyboard;
	this->controllers = new std::vector<Controller>();
}

void ControllerManager::AddController(GameObject* gameObject, ControllerType controllerType, float moveSpeed)
{
	controllers->push_back(Controller(gameObject, this->keyboard, controllerType, moveSpeed));
	gameObject->SetController(&controllers->back());
}

std::vector<Controller>* ControllerManager::GetControllers()
{
	return this->controllers;
}

void ControllerManager::UpdateControllers(float deltaTime)
{
	size_t controllerSize = controllers->size();
	for (size_t i = 0; i < controllerSize; ++i)
	{
		if (controllers->at(i).IsActive())
		{
			controllers->at(i).UpdateObject(deltaTime);
		}
	}
}
