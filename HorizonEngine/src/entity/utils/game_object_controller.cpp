#include "game_object_controller.h"

#include "../../input/input_manager.h"

namespace hrzn::entity
{
	ControllerManager::ControllerManager() :
		m_controllers()
	{
	}

	void ControllerManager::addController(GameObject* gameObject, ControllerType controllerType, float moveSpeed)
	{
		m_controllers.push_back(GameObjectController(gameObject, controllerType, moveSpeed));
		gameObject->setController(&m_controllers.back());
	}

	std::vector<GameObjectController>& ControllerManager::getControllers()
	{
		return m_controllers;
	}

	void ControllerManager::updateControllers(float deltaTime)
	{
		size_t controllerSize = m_controllers.size();
		for (size_t i = 0; i < controllerSize; ++i)
		{
			if (m_controllers.at(i).isActive())
			{
				m_controllers.at(i).updateObject(deltaTime);
			}
		}
	}

	GameObjectController::GameObjectController(GameObject* gameObject, ControllerType controllerType, float moveSpeed) :
		m_gameObject(gameObject),
		m_controllerType(controllerType),

		m_moveSpeed(moveSpeed),
		m_active(false)
	{
	}

	void GameObjectController::updateObject(float deltaTime)
	{
		deltaTime *= 1000.0;

		float speedMod = 1.0f;

		if (InputManager::it().isKeyPressed(VK_SHIFT))
		{
			speedMod = 3.0f;
		}

		switch (m_controllerType)
		{
		case ControllerType::eCamera:
			if (InputManager::it().isKeyPressed('W'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getFrontVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('S'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getBackVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('A'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getLeftVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('D'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getRightVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('E'))
			{
				m_gameObject->getWritableTransform().adjustPosition(0.0f, m_moveSpeed * speedMod * deltaTime, 0.0f);
			}
			if (InputManager::it().isKeyPressed('Q'))
			{
				m_gameObject->getWritableTransform().adjustPosition(0.0f, -m_moveSpeed * speedMod * deltaTime, 0.0f);
			}
			break;

		case ControllerType::eHorizontal:
			if (InputManager::it().isKeyPressed('W'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getFrontVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('S'))
			{
				m_gameObject->getWritableTransform().adjustPosition(m_gameObject->getTransform().getBackVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('A'))
			{
				m_gameObject->getWritableTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -0.3f * m_moveSpeed * speedMod * deltaTime);
			}
			if (InputManager::it().isKeyPressed('D'))
			{
				m_gameObject->getWritableTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.3f * m_moveSpeed * speedMod * deltaTime);
			}
			break;
		}
	}

	bool GameObjectController::isActive() const
	{
		return m_active;
	}

	bool* GameObjectController::isActivePtr()
	{
		return &m_active;
	}

	void GameObjectController::setActive(bool active)
	{
		active = active;
	}

	float GameObjectController::getMoveSpeed() const
	{
		return m_moveSpeed;
	}

	ControllerType GameObjectController::getType() const
	{
		return m_controllerType;
	}

	const GameObject* GameObjectController::getGameObject() const
	{
		return m_gameObject;
	}
}
