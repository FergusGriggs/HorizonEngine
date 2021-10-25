#pragma once

#include "../game_object.h"
#include "../../../input/keyboard/keyboard.h";

namespace hrzn::scene::entity
{
	enum class ControllerType
	{
		eCamera,
		eHorizontal,
	};

	class ControllerManager
	{
	public:
		ControllerManager(input::Keyboard* keyboard);

		void addController(GameObject* gameObject, ControllerType controllerType, float moveSpeed);

		void updateControllers(float deltaTime);
		std::vector<GameObjectController>& getControllers();

	private:
		std::vector<GameObjectController>  m_controllers;
		input::Keyboard* m_keyboard;
	};

	class GameObjectController
	{
	public:
		GameObjectController(GameObject* gameObject, input::Keyboard* keyboard, ControllerType controllerType, float moveSpeed);

		void  updateObject(float deltaTime);
		bool  isActive();
		bool* isActivePtr();
		void  setActive(bool active);

		float          getMoveSpeed();
		ControllerType getType();

		GameObject* getGameObject();

	private:
		GameObject* m_gameObject;

		input::Keyboard* m_keyboard;
		ControllerType   m_controllerType;

		float            m_moveSpeed;
		bool             m_active;
	};
}
