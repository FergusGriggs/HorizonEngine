#pragma once

#include "../game_object.h"

namespace hrzn::entity
{
	enum class ControllerType
	{
		eInvalid,
		eCamera,
		eHorizontal,
	};

	static const std::unordered_map<std::string, ControllerType> sc_controllerTypeStrings = {
		{ "Camera",      ControllerType::eCamera },
		{ "Horizontal", ControllerType::eHorizontal },
	};

	class ControllerManager
	{
	public:
		ControllerManager();

		void addController(GameObject* gameObject, ControllerType controllerType, float moveSpeed, bool active);

		void updateControllers(float deltaTime);
		std::vector<GameObjectController*>& getControllers();

	private:
		std::vector<GameObjectController*>  m_controllers;
	};

	class GameObjectController
	{
	public:
		GameObjectController(GameObject* gameObject, ControllerType controllerType, float moveSpeed, bool active);

		void  updateObject(float deltaTime);
		bool  isActive() const;
		bool* isActivePtr();
		void  setActive(bool active);

		float          getMoveSpeed() const;
		ControllerType getType() const;

		const GameObject* getGameObject() const;

	private:
		GameObject* m_gameObject;
		ControllerType   m_controllerType;

		float            m_moveSpeed;
		bool             m_active;
	};
}
