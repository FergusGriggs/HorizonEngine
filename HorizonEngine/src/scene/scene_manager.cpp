#include "scene_manager.h"

#include "../utils/io_helpers.h"

namespace hrzn::scene
{
	SceneManager::SceneManager(entity::ControllerManager* controllerManager) :
	m_controllerManager(controllerManager)
	{
	}

	SceneManager::~SceneManager()
	{
	}

	bool SceneManager::initialise()
	{
		//dynamic_cast<RenderableGameObject*>(gameObjectMap.at("floor1"))->SetScale(XMFLOAT3(5.0f, 1.0f, 5.0f));

			//springs.push_back(new Spring(XMVectorSet(0.0f, 10.0f, 5.0f, 0.0f), physicsGameObjects.at(0)->GetRigidBody(), 5.0f, 150.0f));
			//springs.push_back(new Spring(physicsGameObjects.at(3)->GetRigidBody(), physicsGameObjects.at(4)->GetRigidBody(), 10.0f, 150.0f));

			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box1"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box2"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box3"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box4"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box5"))->GetRigidBody()->SetIsStatic(false);

		m_particleSystem = new entity::physics::ParticleSystem();
		m_particleSystem->addEmitter(XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.25f, 5.0f, 0.25f, 1.5f, 0.25f, 0.005f, 0.25f);

		if (!m_skybox.initialize("Skybox", "res/models/skyboxes/ocean.obj"))
		{
			return false;
		}

		if (!m_clouds.initialize("Clouds", "res/models/simple_plane.obj"))
		{
			return false;
		}
		m_clouds.getTransform().setPosition(0.0f, 2000.0f, 0.0f);
		m_clouds.setScale(XMFLOAT3(200.0f, 200.0f, 200.0f));

		if (!m_ocean.initialize("Ocean", "res/models/ocean_tesselated.obj"))
		{
			return false;
		}
		m_ocean.getTransform().setPosition(0.0f, -25.0f, 0.0f);

		if (!m_directionalLight.initialize("Directional Light"))
		{
			return false;
		}
		m_directionalLight.getTransform().setPosition(2.0f, 6.0f, 2.0f);
		m_directionalLight.getTransform().lookAtPosition(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_directionalLight.setColour(XMFLOAT3(0.9f, 0.85f, 0.8f));

		m_activeCamera->getTransform().setPosition(-12.0f, 3.0f, 7.0f);
		m_activeCamera->getTransform().lookAtPosition(XMFLOAT3(-12.0f, 0.0f, -3.6f));
		//m_camera.getTransform().lookAtPosition(XMVectorSet(0.0f, 7.0f, 0.0f, 1.0f));
		m_activeCamera->setProjectionValues(90.0f, static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight), 0.1f, 1000.0f);
		//camera.SetObjectTrack(objectTracks.at("camera_track"));
		//camera.SetFollowingObjectTrack(true);
	}

	bool SceneManager::loadScene(const char* sceneName)
	{
		unloadScene();

		m_sceneName = sceneName;

		std::string sceneFilePath = "res/scenes/";
		sceneFilePath += sceneName;

		std::ifstream sceneFile(sceneFilePath.c_str());

		if (sceneFile)
		{
			int numObjectTracks;
			sceneFile >> numObjectTracks;

			for (int i = 0; i < numObjectTracks; ++i)
			{
				std::string trackName;
				sceneFile >> trackName;
				utils::string_helpers::replaceChars(trackName, '|', ' ');

				int numNodes;
				sceneFile >> numNodes;

				entity::GameObjectTrack* track = new entity::GameObjectTrack();

				for (int j = 0; j < numNodes; ++j)
				{
					XMFLOAT3 nodePos = utils::io_helpers::readFloat3(sceneFile);

					XMFLOAT3 nodeLookAt = utils::io_helpers::readFloat3(sceneFile);

					track->addTrackNode(entity::ObjectTrackNode(nodePos, nodeLookAt));
				}

				track->generateMidPoints();

				track->setLabel(trackName);

				m_objectTracks.insert(std::make_pair(trackName, track));
			}

			int numObjects;
			sceneFile >> numObjects;

			for (int i = 0; i < numObjects; ++i)
			{
				entity::GameObject* gameObject;

				int intObjectType;
				sceneFile >> intObjectType;
				entity::GameObjectType objectType = static_cast<entity::GameObjectType>(intObjectType);

				std::string label;
				sceneFile >> label;
				utils::string_helpers::replaceChars(label, '|', ' ');

				switch (objectType)
				{
				case entity::GameObjectType::eRenderable:
				{
					std::string fileName;
					sceneFile >> fileName;
					utils::string_helpers::replaceChars(fileName, '|', ' ');
					fileName = "res/models/" + fileName;

					entity::RenderableGameObject* renderableGameObject = new entity::RenderableGameObject();
					if (!renderableGameObject->initialize(label, fileName))
					{
						std::cout << "Failed to init renderable game object with label " << label << "\n";
						return false;
					}

					gameObject = dynamic_cast<entity::GameObject*>(renderableGameObject);
					break;
				}
				case entity::GameObjectType::eLight:
				{
					if (!m_directionalLight.initialize(label))
					{
						std::cout << "Failed to init directional light with label " << label << "\n";
						return false;
					}

					m_directionalLight.setColour(utils::io_helpers::readFloat3(sceneFile));

					gameObject = dynamic_cast<entity::GameObject*>(&m_directionalLight);

					break;
				}
				case entity::GameObjectType::ePointLight:
				{
					entity::PointLightGameObject* pointLight = new entity::PointLightGameObject();
					m_pointLights.push_back(pointLight);

					gameObject = dynamic_cast<entity::GameObject*>(pointLight);

					if (!pointLight->initialize(label))
					{
						std::cout << "Failed to init point light with label " << label << "\n";
						return false;
					}

					pointLight->setColour(utils::io_helpers::readFloat3(sceneFile));
					break;
				}
				case entity::GameObjectType::eSpotLight:
				{
					entity::SpotLightGameObject* spotLight = new entity::SpotLightGameObject();
					m_spotLights.push_back(spotLight);

					gameObject = dynamic_cast<entity::GameObject*>(spotLight);

					if (!spotLight->initialize(label))
					{
						std::cout << "Failed to init spot light with label " << label << "\n";
						return false;
					}

					spotLight->setColour(utils::io_helpers::readFloat3(sceneFile));

					float innerCutoff, outerCutoff;
					sceneFile >> innerCutoff >> outerCutoff;
					spotLight->setInnerCutoff(innerCutoff);
					spotLight->setOuterCutoff(outerCutoff);
					break;
				}
				case entity::GameObjectType::ePhysics:
				{
					entity::physics::PhysicsGameObject* physicsGameObject = new entity::physics::PhysicsGameObject();
					m_physicsGameObjects.push_back(physicsGameObject);

					gameObject = dynamic_cast<entity::GameObject*>(physicsGameObject);

					std::string fileName;
					sceneFile >> fileName;
					utils::string_helpers::replaceChars(fileName, '|', ' ');
					fileName = "res/models/" + fileName;

					if (!physicsGameObject->initialize(label, fileName))
					{
						std::cout << "Failed to init physics object with label " << label << "\n";
						return false;
					}
					break;
				}
				default:
				{
					gameObject = new entity::GameObject();
					break;
				}
				}

				m_gameObjectMap.insert({ gameObject->getLabel(), gameObject });

				gameObject->getTransform().setPosition(utils::io_helpers::readFloat3(sceneFile));

				int rotationType;
				sceneFile >> rotationType;

				switch (rotationType)
				{
				case 1:
				{
					XMFLOAT3 front = utils::io_helpers::readFloat3(sceneFile);
					XMFLOAT3 right = utils::io_helpers::readFloat3(sceneFile);
					XMFLOAT3 up = utils::io_helpers::readFloat3(sceneFile);

					// Outdated method

					/*gameObject->GetTransform()->SetFrontVector(XMVectorSet(front.x, front.y, front.z, 0.0f));
					gameObject->GetTransform()->SetRightVector(XMVectorSet(right.x, right.y, right.z, 0.0f));
					gameObject->GetTransform()->SetUpVector(XMVectorSet(up.x, up.y, up.z, 0.0f));*/

					break;
				}

				case 2:
					gameObject->getTransform().lookAtPosition(utils::io_helpers::readFloat3(sceneFile));
					break;
				case 3:
				{
					XMFLOAT4 orientation = utils::io_helpers::readFloat4(sceneFile);
					XMVECTOR orientationVector = XMLoadFloat4(&orientation);
					gameObject->getTransform().setOrientationQuaternion(orientationVector);

					break;
				}
				}

				bool hasTrack;
				sceneFile >> hasTrack;

				if (hasTrack)
				{
					std::string trackName;
					sceneFile >> trackName;

					gameObject->setObjectTrack(m_objectTracks.at(trackName));

					bool followingTrack;
					sceneFile >> followingTrack;

					gameObject->setFollowingObjectTrack(followingTrack);

					float objectTrackDelta;
					sceneFile >> objectTrackDelta;

					gameObject->setObjectTrackDelta(objectTrackDelta);
				}

				int numRelativeCams;
				sceneFile >> numRelativeCams;

				for (int i = 0; i < numRelativeCams; ++i)
				{
					XMFLOAT3 cameraRelativePosition = utils::io_helpers::readFloat3(sceneFile);
					gameObject->getRelativePositions()->push_back(DirectX::XMFLOAT3(cameraRelativePosition.x, cameraRelativePosition.y, cameraRelativePosition.z));
				}

				bool hasController;
				sceneFile >> hasController;

				if (hasController)
				{
					int controllerTypeInt;
					sceneFile >> controllerTypeInt;

					float moveSpeed;
					sceneFile >> moveSpeed;

					entity::ControllerType controllerType = static_cast<entity::ControllerType>(controllerTypeInt);
					m_controllerManager->addController(gameObject, controllerType, moveSpeed);
				}

				bool floating;
				sceneFile >> floating;

				gameObject->setFloating(floating);
			}

			return true;
		}

		return false;
	}

	bool SceneManager::saveScene()
	{
		std::string sceneFilePath = "res/scenes/";
		sceneFilePath += m_sceneName;

		std::ofstream sceneFile(sceneFilePath.c_str());

		if (sceneFile)
		{
			try
			{
				sceneFile << m_objectTracks.size() << "\n\n";

				std::unordered_map<std::string, entity::GameObjectTrack*>::iterator objectTrackIterator = m_objectTracks.begin();
				while (objectTrackIterator != m_objectTracks.end())
				{
					std::vector<entity::ObjectTrackNode>* trackNodes = objectTrackIterator->second->getTrackNodes();

					size_t numTrackNodes = trackNodes->size();

					sceneFile << objectTrackIterator->first << ' ' << numTrackNodes << '\n';

					for (size_t i = 0; i < numTrackNodes; ++i)
					{
						utils::io_helpers::writeFloat3(trackNodes->at(i).m_position, sceneFile);
						utils::io_helpers::writeFloat3(trackNodes->at(i).m_lookPoint, sceneFile);

						sceneFile << '\n';
					}

					sceneFile << '\n';

					++objectTrackIterator;
				}

				sceneFile << m_gameObjectMap.size() << "\n\n";

				std::unordered_map<std::string, entity::GameObject*>::iterator objectMapIterator = m_gameObjectMap.begin();
				while (objectMapIterator != m_gameObjectMap.end())
				{
					entity::GameObjectType objectType = objectMapIterator->second->getType();
					int objectTypeInt = static_cast<int>(objectType);
					sceneFile << objectTypeInt << ' ';

					std::string label = objectMapIterator->first;
					utils::string_helpers::replaceChars(label, ' ', '|');
					sceneFile << label << ' ';

					switch (objectType)
					{
					case entity::GameObjectType::eRenderable:
					{
						entity::RenderableGameObject* renderableGameObject = dynamic_cast<entity::RenderableGameObject*>(objectMapIterator->second);
						std::string path = renderableGameObject->getModel()->getPath();
						utils::string_helpers::removeDirectoriesFromStart(path, 2);
						sceneFile << path;
						break;
					}
					case entity::GameObjectType::eLight:
					{
						entity::LightGameObject* directionalLight = dynamic_cast<entity::LightGameObject*>(objectMapIterator->second);
						utils::io_helpers::writeFloat3(directionalLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObjectType::ePointLight:
					{
						entity::PointLightGameObject* pointLight = dynamic_cast<entity::PointLightGameObject*>(objectMapIterator->second);
						utils::io_helpers::writeFloat3(pointLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObjectType::eSpotLight:
					{
						entity::SpotLightGameObject* spotLight = dynamic_cast<entity::SpotLightGameObject*>(objectMapIterator->second);
						utils::io_helpers::writeFloat3(spotLight->getColour(), sceneFile);
						sceneFile << spotLight->getInnerCutoff() << " " << spotLight->getOuterCutoff() << " ";
						break;
					}
					}

					sceneFile << '\n';

					utils::io_helpers::writeFloat3(objectMapIterator->second->getTransform().getPositionFloat3(), sceneFile);
					sceneFile << '\n';

					sceneFile << 3 << ' ';
					utils::io_helpers::writeFloat4(objectMapIterator->second->getTransform().getOrientation(), sceneFile);
					sceneFile << '\n';

					entity::GameObjectTrack* gameObjectTrack = objectMapIterator->second->getObjectTrack();
					if (gameObjectTrack != nullptr)
					{
						sceneFile << 1 << ' ' << gameObjectTrack->getLabel() << ' ' << objectMapIterator->second->getFollowingObjectTrack() << ' ' << objectMapIterator->second->getObjectTrackDelta() << '\n';
					}
					else
					{
						sceneFile << 0 << '\n';
					}

					size_t numRelativeCams = objectMapIterator->second->getRelativePositions()->size();

					sceneFile << numRelativeCams << '\n';

					for (size_t i = 0; i < numRelativeCams; ++i)
					{
						utils::io_helpers::writeFloat3(objectMapIterator->second->getRelativePositions()->at(i), sceneFile);
						sceneFile << '\n';
					}

					if (objectMapIterator->second->getController() != nullptr)
					{
						sceneFile << "1 " << static_cast<int>(objectMapIterator->second->getController()->getType()) << ' ' << objectMapIterator->second->getController()->getMoveSpeed() << '\n';
					}
					else
					{
						sceneFile << "0\n";
					}

					sceneFile << objectMapIterator->second->getFloating() << "\n\n";

					++objectMapIterator;
				}
			}
			catch (std::exception e)
			{
				utils::ErrorLogger::log("Failed to save scene");
				return false;
			}

			return true;
		}

		return false;
	}

	bool SceneManager::saveSceneTGP()
	{
		std::string sceneFilePath = "res/scenes/";
		sceneFilePath = sceneFilePath + "tgp_" + m_sceneName;

		std::ofstream sceneFile(sceneFilePath.c_str());

		if (sceneFile)
		{
			for (int i = 0; i < m_renderableGameObjects.size(); ++i)
			{
				std::string path = m_renderableGameObjects.at(i)->getModel()->getPath();
				utils::string_helpers::removeDirectoriesFromStart(path, 4);

				XMFLOAT3 position = m_renderableGameObjects.at(i)->getTransform().getPositionFloat3();
				XMFLOAT4 orientation = m_renderableGameObjects.at(i)->getTransform().getOrientationFloat4();

				sceneFile << "n " << path.substr(0, path.length() - 5) << "\np " << position.x << " " << position.y << " " << position.z << "\no " << orientation.x << " " << orientation.y << " " << orientation.z << " " << orientation.w << "\n";
			}

			for (int i = 0; i < m_pointLights.size(); ++i)
			{
				XMFLOAT3 position = m_pointLights.at(i)->getTransform().getPositionFloat3();
				XMFLOAT4 orientation = m_pointLights.at(i)->getTransform().getOrientationFloat4();
				XMFLOAT3 colour = m_pointLights.at(i)->getColour();
				sceneFile << "n point\n" << colour.x << " " << colour.y << " " << colour.z << "\np " << position.x << " " << position.y << " " << position.z << "\no " << orientation.x << " " << orientation.y << " " << orientation.z << " " << orientation.w << "\n";
			}

			for (int i = 0; i < m_spotLights.size(); ++i)
			{
				XMFLOAT3 position = m_spotLights.at(i)->getTransform().getPositionFloat3();
				XMFLOAT4 orientation = m_spotLights.at(i)->getTransform().getOrientationFloat4();
				XMFLOAT3 colour = m_spotLights.at(i)->getColour();
				sceneFile << "n spot\n" << colour.x << " " << colour.y << " " << colour.z << " " << m_spotLights.at(i)->getInnerCutoff() << " " << m_spotLights.at(i)->getOuterCutoff() << "\np " << position.x << " " << position.y << " " << position.z << "\no " << orientation.x << " " << orientation.y << " " << orientation.z << " " << orientation.w << "\n";
			}

			sceneFile << "e";
			sceneFile.close();
		}

		return true;
	}

	void SceneManager::unloadScene()
	{
		//Remove Object Tracks
		std::unordered_map<std::string, entity::GameObjectTrack*>::iterator objectTrackIterator = m_objectTracks.begin();
		while (objectTrackIterator != m_objectTracks.end())
		{
			delete objectTrackIterator->second;
			++objectTrackIterator;
		}

		m_objectTracks.clear();

		//Remove Game Objects
		std::unordered_map<std::string, entity::GameObject*>::iterator objectMapIterator = m_gameObjectMap.begin();
		while (objectMapIterator != m_gameObjectMap.end())
		{
			removeGameObject(objectMapIterator->first);
			objectMapIterator = m_gameObjectMap.begin();
		}

		m_gameObjectMap.clear();
		m_renderableGameObjects.clear();
		m_physicsGameObjects.clear();
		m_pointLights.clear();
		m_spotLights.clear();

		if (m_selectedObject != nullptr)
		{
			m_selectedObject = nullptr;
			//m_lastAxisGrabOffset = FLT_MAX;
			//m_lastGrabPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	void SceneManager::update(float deltaTime)
	{
		if (!m_paused)
		{
			m_gameTime += deltaTime;
		}

		if (m_dayNightCycle)
		{
			m_dayProgress += deltaTime * 0.0166666; // 1 day = 60 seconds
		}

		if (m_dayProgress > 1.0f) m_dayProgress -= 1.0f;

		// Update mouse NDC
		computeMouseNDC();

		// Update mouse to world vector
		m_camera.computeMouseToWorldVectorDirection(m_mouseNDCX, m_mouseNDCY);

		// Update selected object (translation / rotation)
		if (m_selectedObject != nullptr)
		{
			updateSelectedObject();
		}

		m_camera.update(deltaTime);

		// Update springs
		for (int i = 0; i < m_springs.size(); ++i)
		{
			m_springs[i]->update();
		}

		size_t numRenderableGameObjects = m_renderableGameObjects.size();
		for (size_t i = 0; i < numRenderableGameObjects; ++i)
		{
			m_renderableGameObjects.at(i)->update(deltaTime);
		}

		checkObjectCollisions(deltaTime);

		size_t numPointLights = m_pointLights.size();
		for (size_t i = 0; i < numPointLights; ++i)
		{
			m_pointLights.at(i)->update(deltaTime);
		}

		size_t numSpotLights = m_spotLights.size();
		for (size_t i = 0; i < numSpotLights; ++i)
		{
			m_spotLights.at(i)->update(deltaTime);
		}

		m_particleSystem->update(deltaTime);
	}

	void SceneManager::checkObjectCollisions(float deltaTime)
	{
	}

	entity::CameraGameObject& SceneManager::getCamera()
	{
		return *m_activeCamera;
	}

	const entity::GameObject& SceneManager::getGameObject(const std::string& label)
	{
		if (m_gameObjectMap.find(label) != m_gameObjectMap.end())
		{
			return *(m_gameObjectMap.at(label));
		}
	}

	entity::GameObject& SceneManager::getWritableGameObject(const std::string& label)
	{
		if (m_gameObjectMap.find(label) != m_gameObjectMap.end())
		{
			return *(m_gameObjectMap.at(label));
		}
	}

	void SceneManager::removeGameObject(const std::string& gameObjectLabel)
	{
		//If the object exists in the map
		std::unordered_map<std::string, entity::GameObject*>::iterator iterator = m_gameObjectMap.find(gameObjectLabel);
		if (iterator != m_gameObjectMap.end())
		{
			//Remove controller
			if (iterator->second->getController() != nullptr)
			{
				std::vector<entity::GameObjectController>& controllers = m_controllerManager->getControllers();
				for (size_t i = 0; i < controllers.size(); ++i)
				{
					if (iterator->second->getController() == &controllers[i])
					{
						controllers.erase(controllers.begin() + i);
						break;
					}
				}
			}

			// Remove from renderables
			for (int i = 0; i < m_renderableGameObjects.size(); ++i)
			{
				if (iterator->second == m_renderableGameObjects[i])
				{
					m_renderableGameObjects.erase(m_renderableGameObjects.begin() + i);
				}
			}

			// Remove from physics
			for (int i = 0; i < m_physicsGameObjects.size(); ++i)
			{
				if (iterator->second == m_physicsGameObjects[i])
				{
					m_physicsGameObjects.erase(m_physicsGameObjects.begin() + i);
				}
			}

			// Remove from spot lights
			for (int i = 0; i < m_spotLights.size(); ++i)
			{
				if (iterator->second == m_spotLights[i])
				{
					m_spotLights.erase(m_spotLights.begin() + i);
				}
			}

			// Remove from point lights
			for (int i = 0; i < m_pointLights.size(); ++i)
			{
				if (iterator->second == m_pointLights[i])
				{
					m_pointLights.erase(m_pointLights.begin() + i);
				}
			}

			delete iterator->second;

			m_gameObjectMap.erase(iterator->first);
		}
	}

	void SceneManager::mouseEventDelegate(const input::MouseEvent& mouseEvent, float deltaTime)
	{
		if (mouseEvent.getType() == input::MouseEvent::EventType::eRawMove)
		{
			if (m_mouse.isRightDown())
			{
				float moveFactor = m_activeCamera->getFOV() / 90.0f;
				if (moveFactor > 1.0f) moveFactor = 1.0f;
				m_activeCamera->getTransform().rotateUsingAxis(m_graphicsHandler.getCamera().getTransform().getRightVector(), (float)mouseEvent.getPos().y * 0.01f * moveFactor);
				m_activeCamera->getTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.getPos().x * 0.01f * moveFactor);
			}
		}

		if (mouseEvent.getType() == input::MouseEvent::EventType::eScrollUp)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				m_activeCamera->zoom(-2.0f);
			}
		}

		if (mouseEvent.getType() == input::MouseEvent::EventType::eScrollDown)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				m_activeCamera->zoom(2.0f);
			}
		}

		if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftPress)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				m_graphicsHandler.checkSelectingObject();
			}
		}
		if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftRelease)
		{
			if (!(m_graphicsHandler.getAxisEditSubState() == gfx::AxisEditSubState::eEditNone))
			{
				m_graphicsHandler.stopAxisEdit();
			}
		}
	}

	void SceneManager::checkSelectingObject()
	{
		float closestDist = FLT_MAX;
		float distance;

		if (m_selectingGameObject)
		{
			XMFLOAT3 objectPos = m_selectedObject->getTransform().getPositionFloat3();

			float objectHitRadius = m_selectedObject->getModel()->getHitRadius();

			//CLAMP SCALE
			if (objectHitRadius < 0.75f) objectHitRadius = 0.75f;

			float camdist = XMVectorGetX(XMVector3Length(m_selectedObject->getTransform().getPositionVector() - m_camera.getTransform().getPositionVector()));
			objectHitRadius *= camdist * 0.5f;

			//TRANSLATE
			if (m_axisEditState == AxisEditState::eEditTranslate)
			{
				//UPDATE D3D COLLISION OBJECTS
				m_xAxisTranslateBoudingBox.Extents = XMFLOAT3(m_xAxisTranslateDefaultBounds.x * objectHitRadius, m_xAxisTranslateDefaultBounds.y * objectHitRadius, m_xAxisTranslateDefaultBounds.z * objectHitRadius);
				m_yAxisTranslateBoudingBox.Extents = XMFLOAT3(m_yAxisTranslateDefaultBounds.x * objectHitRadius, m_yAxisTranslateDefaultBounds.y * objectHitRadius, m_yAxisTranslateDefaultBounds.z * objectHitRadius);
				m_zAxisTranslateBoudingBox.Extents = XMFLOAT3(m_zAxisTranslateDefaultBounds.x * objectHitRadius, m_zAxisTranslateDefaultBounds.y * objectHitRadius, m_zAxisTranslateDefaultBounds.z * objectHitRadius);

				m_xAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x + objectHitRadius * 0.6f, objectPos.y, objectPos.z);
				m_yAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x, objectPos.y + objectHitRadius * 0.6f, objectPos.z);
				m_zAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x, objectPos.y, objectPos.z + objectHitRadius * 0.6f);

				//CHECK IF AN AXIS IS CLICKED ON
				if (m_xAxisTranslateBoudingBox.Intersects(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), distance))
				{
					if (distance < closestDist)
					{
						closestDist = distance;
						m_axisEditSubState = AxisEditSubState::eEditX;
					}
				}
				if (m_yAxisTranslateBoudingBox.Intersects(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), distance))
				{
					if (distance < closestDist)
					{
						closestDist = distance;
						m_axisEditSubState = AxisEditSubState::eEditY;
					}
				}
				if (m_zAxisTranslateBoudingBox.Intersects(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), distance))
				{
					if (distance < closestDist)
					{
						closestDist = distance;
						m_axisEditSubState = AxisEditSubState::eEditZ;
					}
				}

				//STOP FOLLOWING TRACK AND RETURN IF AXIS SELECTED
				if (closestDist != FLT_MAX)
				{
					m_selectedObject->setFollowingObjectTrack(false);
					return;
				}
			}
			//ROTATE
			else if (m_axisEditState == AxisEditState::eEditRotate)
			{
				XMMATRIX modelRotationMatrix = m_selectedObject->getTransform().getRotationMatrix();
				objectHitRadius *= 0.5f;
				{
					//GET TRANSFORMED AXIS VECTOR (PLANE NORMAL)
					XMVECTOR planeNormal = XMVector3Normalize(XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), modelRotationMatrix));
					//GET INTERSECT POINT WITH THIS PLANE AND THE MOUSE RAY
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector(); //FIND VECTOR DIFFERENCE
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));//GET SCALAR DISTANCE
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{ //CHECK IF THE INTERSECT IS ON THE RING
						XMVECTOR camToIntersect = planeIntersectPoint - m_camera.getTransform().getPositionVector(); //FIND VECTOR DIFF FROM CAMERA TO THE POINT
						float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect)); //FIND SCALAR DISTANCE FROM CAMERA TO THE POINT
						if (camToIntersectDist < closestDist)
						{ //IF LESS THAN THE LAST DISTANCE SET VARIABLES
							closestDist = camToIntersectDist;
							m_axisEditSubState = AxisEditSubState::eEditX;
						}
					}
				}
				{
					XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), modelRotationMatrix));
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{
						XMVECTOR camToIntersect = planeIntersectPoint - m_camera.getTransform().getPositionVector();
						float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect));
						if (camToIntersectDist < closestDist)
						{
							closestDist = camToIntersectDist;
							m_axisEditSubState = AxisEditSubState::eEditY;
						}
					}
				}
				{
					XMVECTOR planeNormal = XMVector3Normalize(XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), modelRotationMatrix));
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{
						XMVECTOR camToIntersect = planeIntersectPoint - m_camera.getTransform().getPositionVector();
						float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect));
						if (camToIntersectDist < closestDist)
						{
							closestDist = camToIntersectDist;
							m_axisEditSubState = AxisEditSubState::eEditZ;
						}
					}
				}
				if (closestDist != FLT_MAX)
				{
					m_selectedObject->setFollowingObjectTrack(false);
					return;
				}
			}
		}

		std::unordered_map<std::string, entity::GameObject*>::iterator mapIterator = m_gameObjectMap.begin();

		// Iterate over the map using iterator
		while (mapIterator != m_gameObjectMap.end())
		{
			entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second);
			distance = gameObject->getRayIntersectDist(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection());
			if (distance < closestDist)
			{
				closestDist = distance;
				m_selectedObject = gameObject;
			}
			mapIterator++;
		}

		//IF AN OBJECT WAS SELECTED, SET SELECTING TO TRUE
		if (closestDist != FLT_MAX)
		{
			m_selectingGameObject = true;
		}
		//OTHERWISE, DESELECT THE SELECTED OBJECT
		else
		{
			m_selectingGameObject = false;
			m_selectedObject = nullptr;
			m_axisEditSubState = AxisEditSubState::eEditNone;
			m_lastAxisGrabOffset = FLT_MAX;
		}
	}

	void SceneManager::updateSelectedObject()
	{
		//CHECK MOUSE IS ON SCREEN
		if (m_mouseNDCX > -1.0f && m_mouseNDCX < 1.0f && m_mouseNDCY > -1.0f && m_mouseNDCY < 1.0f)
		{
			//TRANSLATING
			if (m_axisEditState == AxisEditState::eEditTranslate)
			{
				XMFLOAT3 objectPos = m_selectedObject->getTransform().getPositionFloat3();
				switch (m_axisEditSubState)
				{
				case AxisEditSubState::eEditX:
				{
					//GET INTERSECT POINT WITH MOUSE RAY
					XMVECTOR intersect = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetX(intersect); //GET RELEVANT VECTOR COMPONENT
					if (m_lastAxisGrabOffset != FLT_MAX)
					{ //CHECK IF FIRST ITERATION
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset; //FIND DIFFERENCE
						m_selectedObject->getTransform().adjustPosition(diff, 0.0f, 0.0f); //MOVE OBJECT BY DIFFERENCE
					}
					m_lastAxisGrabOffset = currentAxisGrabOffset; //SET LAST OFFSET
					break;
				}
				case AxisEditSubState::eEditY:
				{
					XMVECTOR intersect = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), XMVector3Normalize(XMVectorSet(m_camera.getTransform().getPositionFloat3().x - objectPos.x, 0.0f, m_camera.getTransform().getPositionFloat3().z - objectPos.z, 0.0f)), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetY(intersect);
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset;
						m_selectedObject->getTransform().adjustPosition(0.0f, diff, 0.0f);
					}
					m_lastAxisGrabOffset = currentAxisGrabOffset;
					break;
				}
				case AxisEditSubState::eEditZ:
				{
					XMVECTOR intersect = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetZ(intersect);
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset;
						m_selectedObject->getTransform().adjustPosition(0.0f, 0.0f, diff);
					}
					m_lastAxisGrabOffset = currentAxisGrabOffset;
					break;
				}
				}
			}
			//ROTATING
			else if (m_axisEditState == AxisEditState::eEditRotate)
			{
				XMFLOAT3 objectPos = m_selectedObject->getTransform().getPositionFloat3();
				XMMATRIX modelRotationMatrix = m_selectedObject->getTransform().getRotationMatrix();
				XMMATRIX inverseModelRotationMatrix = XMMatrixInverse(nullptr, modelRotationMatrix);

				switch (m_axisEditSubState)
				{
				case AxisEditSubState::eEditX:
				{
					//COMPUTE WORLD SPACE AXIS VECTOR (PLANE NORMAL)
					XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), modelRotationMatrix));
					//GET INTERSECT POINT OF THIS PLANE WITH MOUSE RAY
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector(); //COMPUTE DIFFERENCE
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix); //TRANSFORM TO MODEL SPACE
					float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetZ(modelSpaceCentreDiff)); //WORK OUT ANGLE OF ROTATION
					if (m_lastAxisGrabOffset != FLT_MAX)
					{ //IF NOT FIRST ITERATION
						float rotationDiff = rotation - m_lastAxisGrabOffset; //FIND ANGLE DIFF
						m_selectedObject->getTransform().rotateUsingAxis(m_selectedObject->getTransform().getRightVector(), -rotationDiff); //ROTATE AXIS BY DIFFERENCE
						m_lastAxisGrabOffset = rotation - rotationDiff; //SET LAST OFFSET
					}
					else
					{
						m_lastAxisGrabOffset = rotation;
					}
					break;
				}
				case AxisEditSubState::eEditY:
				{
					XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), modelRotationMatrix));
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
					float rotation = atan2(XMVectorGetZ(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float rotationDiff = rotation - m_lastAxisGrabOffset;
						m_selectedObject->getTransform().rotateUsingAxis(m_selectedObject->getTransform().getUpVector(), -rotationDiff);
						m_lastAxisGrabOffset = rotation - rotationDiff;
					}
					else
					{
						m_lastAxisGrabOffset = rotation;
					}
					break;
				}
				case AxisEditSubState::eEditZ:
				{
					XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), modelRotationMatrix));
					XMVECTOR planeIntersectPoint = rayPlaneIntersect(m_camera.getTransform().getPositionVector(), m_camera.getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
					float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float rotationDiff = rotation - m_lastAxisGrabOffset;
						m_selectedObject->getTransform().rotateUsingAxis(m_selectedObject->getTransform().getFrontVector(), rotationDiff);
						m_lastAxisGrabOffset = rotation - rotationDiff;
					}
					else
					{
						m_lastAxisGrabOffset = rotation;
					}
					break;
				}
				}
			}
		}
	}

	float SceneManager::getWaterHeightAt(float posX, float posZ, bool exact)
	{
		return 0.0f;
	}

	XMVECTOR SceneManager::getFourierOffset(float x, float z)
	{
		auto hash11 = [](float p)
		{
			float buffer;
			p = modf(p * 0.1031f, &buffer);
			p *= p + 33.33f;
			p *= p + p;
			return modf(p, &buffer);
		};

		XMVECTOR windDir = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR flatPosition = XMVectorSet(x, 0.0f, x, 1.0f);
		XMVECTOR finalOffset = XMVectorSet(0.0f, m_ocean.getTransform().getPositionFloat3().y, 0.0f, 1.0f);

		float scale = 0.5f;

		const config::OceanConfig& oceanConfig = m_sceneConfig.getOceanConfig();

		int waveNum = 0;
		while (waveNum < m_sceneConfig.getOceanConfig().m_waveCount)
		{
			float waveAngle = hash11((float)waveNum * oceanConfig.m_waveSeed) * oceanConfig.m_waveSeed;
			XMVECTOR waveDir = XMVectorSet(cos(waveAngle), 0.0f, sin(waveAngle), 0.0f);
			//float3 waveDirRight = float3(waveDir.z, 0.0f, -waveDir.x);

			float windScaleModifier = XMVectorGetX(XMVector3Dot(waveDir, windDir)) * 0.1f + 0.8f;

			float initialWaveDist = XMVectorGetX(XMVector3Dot(flatPosition, waveDir));
			float distWaveTravelled = m_gameTime * oceanConfig.m_waveSpeed * ((float)waveNum * 1.0f + 1.0f) * scale + initialWaveDist;

			float angle = distWaveTravelled / (oceanConfig.m_wavePeriod * scale * pow(1.1f, (float)waveNum - 1.0f));

			//float signedDistanceToWaveCentre = dot(waveDirRight, flatPosition);
			float waveBreakScaleMod = 1.0f;// sin(signedDistanceToWaveCentre * 0.05f + waveAngle * 1024.0f + gameTime * waveSpeed * 0.06f + initialWaveDist * 0.2f) * 0.15f + 0.85f;

			float xOffset = cos(waveAngle) * cos(angle) * oceanConfig.m_waveScale * scale * waveBreakScaleMod * windScaleModifier;
			float yOffset = sin(angle) * oceanConfig.m_waveScale * scale * waveBreakScaleMod * windScaleModifier;
			float zOffset = sin(waveAngle) * cos(angle) * oceanConfig.m_waveScale * scale * waveBreakScaleMod * windScaleModifier;

			finalOffset += XMVectorSet(xOffset, yOffset, zOffset, 0.0f);

			scale *= oceanConfig.m_waveScaleMultiplier;

			waveNum++;
		}

		return finalOffset;
	}

	void SceneManager::floatObject(entity::GameObject* object)
	{
		const XMFLOAT3& anchorPosition = object->getTransform().getPositionFloat3();

		XMVECTOR objectFront = object->getTransform().getFrontVector();
		XMVECTOR objectRight = object->getTransform().getRightVector();

		XMVECTOR flatTangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectRight), 0.0f, XMVectorGetZ(objectRight), 0.0f));
		XMVECTOR flatBitangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectFront), 0.0f, XMVectorGetZ(objectFront), 0.0f));

		XMVECTOR displacedMainPosition = getFourierOffset(anchorPosition.x, anchorPosition.z);
		XMVECTOR displacedTangentPosition = getFourierOffset(anchorPosition.x + XMVectorGetX(flatTangent), anchorPosition.z + XMVectorGetZ(flatTangent));
		XMVECTOR displacedBitangentPosition = getFourierOffset(anchorPosition.x + XMVectorGetX(flatBitangent), anchorPosition.z + XMVectorGetZ(flatBitangent));

		XMStoreFloat3(&(object->getFloatOffset()), displacedMainPosition);

		XMVECTOR tangent = XMVector3Normalize(displacedTangentPosition - displacedMainPosition);
		XMVECTOR bitangent = XMVector3Normalize(displacedBitangentPosition - displacedMainPosition);
		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(bitangent, tangent));

		XMMATRIX rotationMatrix = XMMATRIX(tangent, normal, bitangent, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		rotationMatrix.r[0].m128_f32[3] = 0.0f;
		rotationMatrix.r[1].m128_f32[3] = 0.0f;
		rotationMatrix.r[2].m128_f32[3] = 0.0f;

		//object->getTransform().setOrientationRotationMatrix(rotationMatrix);
	}
}
