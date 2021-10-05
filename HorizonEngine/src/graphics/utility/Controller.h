#pragma once

#include "../GameObject.h"
#include "../input/keyboard/Keyboard.h";

enum class ControllerType
{
	CAMERA,
	HORIZONTAL
};

class ControllerManager
{
public:
	ControllerManager(Keyboard* keyboard);

	void AddController(GameObject* gameObject, ControllerType controllerType, float moveSpeed);
	std::vector<Controller>* GetControllers();
	void UpdateControllers(float deltaTime);

private:
	Keyboard* keyboard;
	std::vector<Controller>* controllers;
};

class Controller
{
public:
	Controller(GameObject* gameObject, Keyboard* keyboard, ControllerType controllerType, float moveSpeed);
	void UpdateObject(float deltaTime);
	bool IsActive();
	bool* IsActivePtr();
	void SetActive(bool active);

	float GetMoveSpeed();
	ControllerType GetType();

	GameObject* GetGameObject();
private:
	GameObject* gameObject;
	Keyboard* keyboard;
	ControllerType controllerType;
	float moveSpeed;
	bool active;
};

