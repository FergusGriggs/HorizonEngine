#include "game_object_controller.h"

namespace hrzn::entity
{
	ControllerManager::ControllerManager(input::Keyboard* keyboard) :
		m_controllers(),
		m_keyboard(keyboard)
	{
	}

	void ControllerManager::addController(GameObject* gameObject, ControllerType controllerType, float moveSpeed)
	{
		m_controllers.push_back(GameObjectController(gameObject, m_keyboard, controllerType, moveSpeed));
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

	GameObjectController::GameObjectController(GameObject* gameObject, input::Keyboard* keyboard, ControllerType controllerType, float moveSpeed) :
		m_gameObject(gameObject),
		m_keyboard(keyboard),
		m_controllerType(controllerType),

		m_moveSpeed(moveSpeed),
		m_active(false)
	{
	}

	void GameObjectController::updateObject(float deltaTime)
	{
		deltaTime *= 1000.0;

		float speedMod = 1.0f;

		if (m_keyboard->keyIsPressed(VK_SHIFT))
		{
			speedMod = 3.0f;
		}

		switch (m_controllerType)
		{
		case ControllerType::eCamera:
			if (m_keyboard->keyIsPressed('W'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getFrontVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('S'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getBackVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('A'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getLeftVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('D'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getRightVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('E'))
			{
				m_gameObject->getTransform().adjustPosition(0.0f, m_moveSpeed * speedMod * deltaTime, 0.0f);
			}
			if (m_keyboard->keyIsPressed('Q'))
			{
				m_gameObject->getTransform().adjustPosition(0.0f, -m_moveSpeed * speedMod * deltaTime, 0.0f);
			}
			break;

		case ControllerType::eHorizontal:
			if (m_keyboard->keyIsPressed('W'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getFrontVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('S'))
			{
				m_gameObject->getTransform().adjustPosition(m_gameObject->getTransform().getBackVector() * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('A'))
			{
				m_gameObject->getTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -0.3f * m_moveSpeed * speedMod * deltaTime);
			}
			if (m_keyboard->keyIsPressed('D'))
			{
				m_gameObject->getTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.3f * m_moveSpeed * speedMod * deltaTime);
			}
			break;
		}
	}

	bool GameObjectController::isActive()
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

	float GameObjectController::getMoveSpeed()
	{
		return m_moveSpeed;
	}

	ControllerType GameObjectController::getType()
	{
		return m_controllerType;
	}

	GameObject* GameObjectController::getGameObject()
	{
		return m_gameObject;
	}
}
