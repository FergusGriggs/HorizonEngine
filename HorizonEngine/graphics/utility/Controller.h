#pragma once

#include "../GameObject.h"
#include "../input/keyboard/Keyboard.h";

enum class ControllerType
{
	CAMERA,
	HORIZONTAL
};

class Controller
{
public:
	Controller(GameObject* gameObject, Keyboard* keyboard, ControllerType controllerType, float moveSpeed);
	void UpdateObject(float deltaTime);
	bool IsActive();
	bool* IsActivePtr();
	void SetActive(bool active);
	GameObject* GetGameObject();
private:
	GameObject* gameObject;
	Keyboard* keyboard;
	ControllerType controllerType;
	float moveSpeed;
	bool active;
};

