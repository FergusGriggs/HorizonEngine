#include "scene_manager.h"

#include "../utils/io_helpers.h"
#include "../utils/error_logger.h"

#include "../user_config.h"

#include "../physics/utils/collision_helpers.h"

#include "../input/input_manager.h"

#include "../graphics/imgui/imgui.h"

namespace hrzn::scene
{
	SceneManager::SceneManager(entity::ControllerManager* controllerManager) :
		m_sceneName("unloaded"),

		m_sceneLoader(this),

		m_sceneConfig(),

		m_activeCamera(nullptr),
		
		m_skybox(),
		m_clouds(),
		m_ocean(),
		
		m_gameObjectMap(),

		m_cameras(),
		m_renderables(),
		m_physicsObjects(),
		m_springs(),

		m_directionalLight(),
		m_pointLights(),
		m_spotLights(),
		
		m_selectedObject(nullptr),

		m_axisTranslateDefaultBounds(),
		m_axisTranslateBoudingBoxes(),

		m_axisEditState(AxisEditState::eEditTranslate),
		m_axisEditSubState(AxisEditSubState::eEditNone),
		
		m_lastAxisGrabOffset(FLT_MAX),
		m_lastGrabPos(XMVectorZero()),

		m_particleSystem(nullptr),
		
		m_controllerManager(controllerManager),
		
		m_objectTracks(),
		
		m_dayNightCycle(false),
		m_paused(false),
		m_dayProgress(0.335f),//0.494f
		m_gameTime(0.0f),
		
		m_newObjectType(0),
		m_newObjectMenuOpen(false),
		m_newObjectLabel(""),
		m_newObjectModelPath("res/models/")
	{
	}

	SceneManager::~SceneManager()
	{
	}

	bool SceneManager::initialise()
	{
		m_particleSystem = new physics::ParticleSystem();
		m_particleSystem->addEmitter(XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.25f, 5.0f, 0.25f, 1.5f, 0.25f, 0.005f, 0.25f);

		if (!m_skybox.initialize("Skybox", "res/models/skyboxes/ocean.obj"))
		{
			return false;
		}

		if (!m_clouds.initialize("Clouds", "res/models/simple_plane.obj"))
		{
			return false;
		}
		m_clouds.getWritableTransform().setPosition(0.0f, 2000.0f, 0.0f);
		m_clouds.setScale(XMFLOAT3(200.0f, 200.0f, 200.0f));

		if (!m_ocean.initialize("Ocean", "res/models/ocean_tesselated.obj"))
		{
			return false;
		}
		m_ocean.getWritableTransform().setPosition(0.0f, -25.0f, 0.0f);

		m_directionalLight.setLabel("directional_light");

		if (!m_directionalLight.initialize())
		{
			return false;
		}
		m_directionalLight.getWritableTransform().setPosition(2.0f, 6.0f, 2.0f);
		m_directionalLight.getWritableTransform().lookAtPosition(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_directionalLight.setColour(XMFLOAT3(0.9f, 0.85f, 0.8f));

		// Set translate axis default bounds / extents
		m_axisTranslateDefaultBounds[0] = XMFLOAT3(0.45f, 0.05f, 0.05f);
		m_axisTranslateDefaultBounds[1] = XMFLOAT3(0.05f, 0.45f, 0.05f);
		m_axisTranslateDefaultBounds[2] = XMFLOAT3(0.05f, 0.05f, 0.45f);

		// Register input delegates
		InputManager::it().registerMouseButtonDelegate(std::bind(&SceneManager::mouseButtonDelegate, this, std::placeholders::_1, std::placeholders::_2));
		InputManager::it().registerMouseScrollDelegate(std::bind(&SceneManager::mouseScrollDelegate, this, std::placeholders::_1, std::placeholders::_2));
		InputManager::it().registerMouseMoveDelegate(std::bind(&SceneManager::mouseMoveDelegate, this, std::placeholders::_1, std::placeholders::_2));

		if (!loadScene("test"))
		{
			return false;
		}
	}

	const std::string& SceneManager::getSceneName()
	{
		return m_sceneName;
	}

	bool SceneManager::loadScene(const char* sceneName)
	{
		unloadScene();

		loadSceneStaticObjects();

		m_sceneLoader.loadScene(sceneName);

		m_dayNightCycle = m_sceneConfig.getTimeConfig().m_dayNightCycle;
		m_dayProgress = m_sceneConfig.getTimeConfig().m_startTime;
		m_paused = m_sceneConfig.getTimeConfig().m_timePaused;

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
				std::string trackId;
				sceneFile >> trackId;
				utils::string_helpers::replaceChars(trackId, '|', ' ');

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

				track->setId(trackId);

				m_objectTracks.insert(std::make_pair(trackId, track));
			}

			int numObjects;
			sceneFile >> numObjects;

			for (int i = 0; i < numObjects; ++i)
			{
				entity::GameObject* gameObject;

				int intObjectType;
				sceneFile >> intObjectType;
				entity::GameObject::Type objectType = static_cast<entity::GameObject::Type>(intObjectType);

				std::string label;
				sceneFile >> label;
				utils::string_helpers::replaceChars(label, '|', ' ');

				switch (objectType)
				{
				case entity::GameObject::Type::eRenderable:
				{
					entity::RenderableGameObject* renderableGameObject = new entity::RenderableGameObject();
					gameObject = dynamic_cast<entity::GameObject*>(renderableGameObject);

					std::string fileName;
					sceneFile >> fileName;
					utils::string_helpers::replaceChars(fileName, '|', ' ');
					fileName = "res/models/" + fileName;

					if (!renderableGameObject->initialize(label, fileName))
					{
						std::cout << "Failed to init renderable game object with label " << label << "\n";
						return false;
					}
					
					break;
				}
				case entity::GameObject::Type::eLight:
				{
					if (!m_directionalLight.initialize())
					{
						std::cout << "Failed to init directional light with label " << label << "\n";
						return false;
					}

					m_directionalLight.setColour(utils::io_helpers::readFloat3(sceneFile));

					gameObject = dynamic_cast<entity::GameObject*>(&m_directionalLight);
					break;
				}
				case entity::GameObject::Type::ePointLight:
				{
					entity::PointLightGameObject* pointLight = new entity::PointLightGameObject();
					gameObject = dynamic_cast<entity::GameObject*>(pointLight);

					if (!pointLight->initialize())
					{
						std::cout << "Failed to init point light with label " << label << "\n";
						return false;
					}

					pointLight->setColour(utils::io_helpers::readFloat3(sceneFile));
					break;
				}
				case entity::GameObject::Type::eSpotLight:
				{
					entity::SpotLightGameObject* spotLight = new entity::SpotLightGameObject();
					gameObject = dynamic_cast<entity::GameObject*>(spotLight);

					if (!spotLight->initialize())
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
				case entity::GameObject::Type::ePhysics:
				{
					entity::PhysicsGameObject* physicsGameObject = new entity::PhysicsGameObject();
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

				addGameObject(gameObject);

				gameObject->getWritableTransform().setPosition(utils::io_helpers::readFloat3(sceneFile));

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

					/*gameObject->getTransform()->SetFrontVector(XMVectorSet(front.x, front.y, front.z, 0.0f));
					gameObject->getTransform()->SetRightVector(XMVectorSet(right.x, right.y, right.z, 0.0f));
					gameObject->getTransform()->SetUpVector(XMVectorSet(up.x, up.y, up.z, 0.0f));*/

					break;
				}

				case 2:
					gameObject->getWritableTransform().lookAtPosition(utils::io_helpers::readFloat3(sceneFile));
					break;
				case 3:
				{
					XMFLOAT4 orientation = utils::io_helpers::readFloat4(sceneFile);
					XMVECTOR orientationVector = XMLoadFloat4(&orientation);
					gameObject->getWritableTransform().setOrientationQuaternion(orientationVector);

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
					gameObject->getWritableRelativePositions().push_back(DirectX::XMFLOAT3(cameraRelativePosition.x, cameraRelativePosition.y, cameraRelativePosition.z));
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

	bool SceneManager::loadSceneStaticObjects()
	{
		/*entity::PhysicsGameObject* physicsObject = new entity::PhysicsGameObject();
		physicsObject->initialize("box1", "res/models/test_cubes/bricks.obj");

		entity::PhysicsGameObject* physicsObject2 = new entity::PhysicsGameObject();
		physicsObject2->initialize("box2", "res/models/test_cubes/bricks.obj");

		addGameObject(physicsObject);
		addGameObject(physicsObject2);

		m_springs.push_back(new physics::Spring(XMVectorSet(0.0f, 10.0f, 5.0f, 0.0f), m_physicsObjects.at(0)->getRigidBody(), 5.0f, 150.0f));
		m_springs.push_back(new physics::Spring(m_physicsObjects.at(0)->getRigidBody(), m_physicsObjects.at(1)->getRigidBody(), 10.0f, 150.0f));

		dynamic_cast<entity::PhysicsGameObject*>(m_gameObjectMap.at("box1"))->getRigidBody()->setIsStatic(false);
		dynamic_cast<entity::PhysicsGameObject*>(m_gameObjectMap.at("box2"))->getRigidBody()->setIsStatic(false);*/

		// Standard Camera
		entity::CameraGameObject* mainCamera = new entity::CameraGameObject();
		mainCamera->setLabel("main_cam");

		mainCamera->getWritableTransform().setPosition(-12.0f, 3.0f, 7.0f);
		mainCamera->getWritableTransform().lookAtPosition(XMFLOAT3(-12.0f, 0.0f, -3.6f));
		//m_camera.getTransform().lookAtPosition(XMVectorSet(0.0f, 7.0f, 0.0f, 1.0f));
		mainCamera->setProjectionValues(90.0f, UserConfig::it().getWindowAspectRatio(), 0.1f, 1000.0f);
		//camera.SetObjectTrack(objectTracks.at("camera_track"));
		//camera.SetFollowingObjectTrack(true);
		addGameObject(mainCamera);
		m_activeCamera = mainCamera;

		return true;
	}

	bool SceneManager::saveScene(const char* sceneName)
	{
		std::string sceneFilePath = "res/scenes/";
		sceneFilePath += sceneName;

		std::ofstream sceneFile(sceneFilePath.c_str());

		if (sceneFile)
		{
			try
			{
				sceneFile << m_objectTracks.size() << "\n\n";

				std::unordered_map<std::string, entity::GameObjectTrack*>::iterator objectTrackIterator = m_objectTracks.begin();
				while (objectTrackIterator != m_objectTracks.end())
				{
					const std::vector<entity::ObjectTrackNode>& trackNodes = objectTrackIterator->second->getTrackNodes();

					size_t numTrackNodes = trackNodes.size();

					sceneFile << objectTrackIterator->first << ' ' << numTrackNodes << '\n';

					for (size_t i = 0; i < numTrackNodes; ++i)
					{
						utils::io_helpers::writeFloat3(trackNodes[i].m_position, sceneFile);
						utils::io_helpers::writeFloat3(trackNodes[i].m_lookPoint, sceneFile);

						sceneFile << '\n';
					}

					sceneFile << '\n';

					++objectTrackIterator;
				}

				sceneFile << m_gameObjectMap.size() << "\n\n";

				std::unordered_map<std::string, entity::GameObject*>::iterator objectMapIterator = m_gameObjectMap.begin();
				while (objectMapIterator != m_gameObjectMap.end())
				{
					entity::GameObject::Type objectType = objectMapIterator->second->getType();
					int objectTypeInt = static_cast<int>(objectType);
					sceneFile << objectTypeInt << ' ';

					std::string label = objectMapIterator->first;
					utils::string_helpers::replaceChars(label, ' ', '|');
					sceneFile << label << ' ';

					switch (objectType)
					{
					case entity::GameObject::Type::eRenderable:
					{
						entity::RenderableGameObject* renderableGameObject = dynamic_cast<entity::RenderableGameObject*>(objectMapIterator->second);
						std::string path = renderableGameObject->getModel().getPath();
						utils::string_helpers::removeDirectoriesFromStart(path, 2);
						sceneFile << path;
						break;
					}
					case entity::GameObject::Type::eLight:
					{
						entity::LightGameObject* directionalLight = dynamic_cast<entity::LightGameObject*>(objectMapIterator->second);
						utils::io_helpers::writeFloat3(directionalLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObject::Type::ePointLight:
					{
						entity::PointLightGameObject* pointLight = dynamic_cast<entity::PointLightGameObject*>(objectMapIterator->second);
						utils::io_helpers::writeFloat3(pointLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObject::Type::eSpotLight:
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

					const entity::GameObjectTrack* gameObjectTrack = objectMapIterator->second->getObjectTrack();
					if (gameObjectTrack != nullptr)
					{
						sceneFile << 1 << ' ' << gameObjectTrack->getId() << ' ' << objectMapIterator->second->isFollowingObjectTrack() << ' ' << objectMapIterator->second->getObjectTrackDelta() << '\n';
					}
					else
					{
						sceneFile << 0 << '\n';
					}

					size_t numRelativeCams = objectMapIterator->second->getRelativePositions().size();

					sceneFile << numRelativeCams << '\n';

					for (size_t i = 0; i < numRelativeCams; ++i)
					{
						utils::io_helpers::writeFloat3(objectMapIterator->second->getRelativePositions()[i], sceneFile);
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

		if (objectIsSelected())
		{
			m_selectedObject = nullptr;
			m_lastAxisGrabOffset = FLT_MAX;
			m_lastGrabPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
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
			m_dayProgress += deltaTime * (1.0f / m_sceneConfig.getTimeConfig().m_dayDuration); // 1 day = 60 seconds
		}

		if (m_dayProgress > 1.0f) m_dayProgress -= 1.0f;

		// Update mouse to world vector
		m_activeCamera->updateMouseToWorldVectorDirection();

		// Update selected object (translation / rotation)
		if (objectIsSelected())
		{
			updateSelectedObject();
		}

		m_activeCamera->update(deltaTime);

		// Update springs
		for (int i = 0; i < m_springs.size(); ++i)
		{
			m_springs[i]->update();
		}

		size_t numRenderableGameObjects = m_renderables.size();
		for (size_t i = 0; i < numRenderableGameObjects; ++i)
		{
			m_renderables[i]->update(deltaTime);

			if (m_renderables[i]->getFloating())
			{
				floatObject(m_renderables[i]);
			}
		}

		checkObjectCollisions(deltaTime);

		size_t numPointLights = m_pointLights.size();
		for (size_t i = 0; i < numPointLights; ++i)
		{
			m_pointLights[i]->update(deltaTime);
		}

		size_t numSpotLights = m_spotLights.size();
		for (size_t i = 0; i < numSpotLights; ++i)
		{
			m_spotLights[i]->update(deltaTime);
		}

		m_particleSystem->update(deltaTime);
	}

	void SceneManager::checkObjectCollisions(float deltaTime)
	{
		for (int i = 0; i < m_physicsObjects.size(); ++i)
		{
			if (!m_physicsObjects[i]->getRigidBody()->isStatic())
			{
				for (int j = i + 1; j < m_physicsObjects.size(); ++j)
				{
					if (m_physicsObjects[i]->getWorldSpaceBoundingBox().Intersects(m_physicsObjects[j]->getWorldSpaceBoundingBox()))
					{
						float velocityOne = std::max(XMVectorGetX(XMVector3Length(m_physicsObjects[i]->getRigidBody()->getVelocityVector())), 1.0f);
						float velocityTwo = std::max(XMVectorGetX(XMVector3Length(m_physicsObjects[j]->getRigidBody()->getVelocityVector())), 1.0f);

						float forceMagnitude = (m_physicsObjects[i]->getMass() * velocityOne + m_physicsObjects[j]->getMass() * velocityTwo) / deltaTime;
						XMVECTOR force = XMVector3Normalize(m_physicsObjects[j]->getTransform().getPositionVector() - m_physicsObjects[i]->getTransform().getPositionVector()) * forceMagnitude * 0.15f; //remove force (coefficient of restitution)
						m_physicsObjects[i]->getRigidBody()->addForce(-force);
						m_physicsObjects[j]->getRigidBody()->addForce(force);

						/*XMVECTOR relativeOne = 0.5f * (physicsGameObjects[j]->getTransform()->GetPositionVector() - physicsGameObjects[i]->getTransform()->GetPositionVector());
						XMVECTOR forceOne = physicsGameObjects[i]->GetRigidBody()->GetForceAtRelativePosition(relativeOne);

						XMVECTOR relativeTwo = 0.5f * (physicsGameObjects[i]->getTransform()->GetPositionVector() - physicsGameObjects[j]->getTransform()->GetPositionVector());
						XMVECTOR forceTwo = physicsGameObjects[j]->GetRigidBody()->GetForceAtRelativePosition(relativeTwo);

						physicsGameObjects[i]->GetRigidBody()->AddForce(-force);
						physicsGameObjects[j]->GetRigidBody()->AddForce(force);

						physicsGameObjects[i]->GetRigidBody()->AddTorque(relativeOne, (forceTwo * 0.5f - forceOne * 0.5f) * 0.005f);
						physicsGameObjects[j]->GetRigidBody()->AddTorque(relativeTwo, (forceOne * 0.5f - forceTwo * 0.5f) * 0.005f);*/
					}
				}

				XMFLOAT3 objectPosition = m_physicsObjects[i]->getTransform().getPositionFloat3();

				const std::vector<XMFLOAT3>& vertices = m_physicsObjects[i]->getModel().getVertices();
				for (int j = 0; j < vertices.size(); ++j)
				{
					XMFLOAT3 vertexPosition = vertices[j];
					XMVECTOR rotatedPosition = XMVector3Transform(XMLoadFloat3(&vertexPosition), m_physicsObjects[i]->getTransform().getRotationMatrix());
					XMStoreFloat3(&vertexPosition, rotatedPosition);

					float diff = objectPosition.y + vertexPosition.y + 2.0f;
					if (diff < 0.0f)
					{
						m_physicsObjects[i]->getWritableTransform().setPosition(m_physicsObjects[i]->getTransform().getPositionVector() + XMVectorSet(0.0f, -diff, 0.0f, 0.0f));
						objectPosition = m_physicsObjects[i]->getTransform().getPositionFloat3();

						float force = (XMVectorGetX(XMVector3Length(m_physicsObjects[i]->getRigidBody()->getVelocityVector())) * m_physicsObjects[i]->getRigidBody()->getMass()) / deltaTime;
						//float force = XMVectorGetY(physicsGameObjects[i]->GetRigidBody()->GetForceAtRelativePosition(rotatedPosition));
						m_physicsObjects[i]->getRigidBody()->addForce(XMVectorSet(0.0f, force * 0.8f, 0.0f, 0.0f));
						m_physicsObjects[i]->getRigidBody()->addTorque(rotatedPosition, XMVectorSet(0.0f, force * 0.005f, 0.0f, 0.0f));
						//float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(force), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f)));
						//if (dot > 0.0f)
						//{
						//	physicsGameObjects[i]->GetRigidBody()->AddForce(force * dot);//physicsGameObjects[i]->getTransform()->GetPositionVector() + rotatedPosition, 
						//	//physicsGameObjects[i]->GetRigidBody()->AddTorque(rotatedPosition, -force * 0.005f);
						//}
					}
				}

				float upthrustMagnitude = (m_physicsObjects[i]->getRigidBody()->getMass() * 18.0f) / static_cast<float>(vertices.size());
				for (int j = 0; j < vertices.size(); ++j)
				{
					XMFLOAT3 vertexPosition = vertices[j];
					XMVECTOR rotatedPosition = XMVector3Transform(XMLoadFloat3(&vertexPosition), m_physicsObjects[i]->getTransform().getRotationMatrix());
					XMStoreFloat3(&vertexPosition, rotatedPosition);

					float diff = objectPosition.y + vertexPosition.y - getWaterHeightAt(objectPosition.x + vertexPosition.x, objectPosition.z + vertexPosition.z, true);
					if (diff < 0.0f)
					{
						//Buoyancy forces
						m_physicsObjects[i]->getRigidBody()->addForce(XMVectorSet(0.0f, upthrustMagnitude, 0.0f, 0.0f));
						m_physicsObjects[i]->getRigidBody()->addTorque(rotatedPosition, XMVectorSet(0.0f, upthrustMagnitude * 0.001f, 0.0f, 0.0f));

						//Water drag forces
						XMVECTOR waterDragForce = m_physicsObjects[i]->getRigidBody()->getMass() * -m_physicsObjects[i]->getRigidBody()->getVelocityVector() * 0.1f;
						m_physicsObjects[i]->getRigidBody()->addForce(waterDragForce);
					}
				}
			}
		}
	}

	const std::unordered_map<std::string, entity::GameObject*>& SceneManager::getObjectMap() const
	{
		return m_gameObjectMap;
	}

	std::unordered_map<std::string, entity::GameObject*>& SceneManager::getWritableObjectMap()
	{
		return m_gameObjectMap;
	}

	const entity::CameraGameObject& SceneManager::getActiveCamera() const
	{
		return *m_activeCamera;
	}

	void SceneManager::setActiveCamera(entity::CameraGameObject* camera)
	{
		m_activeCamera = camera;
	}

	entity::CameraGameObject& SceneManager::getWritableActiveCamera()
	{
		return *m_activeCamera;
	}

	const std::vector<entity::CameraGameObject*>& SceneManager::getCameraList() const
	{
		return m_cameras;
	}

	std::vector<entity::CameraGameObject*>& SceneManager::getWritableCameraList()
	{
		return m_cameras;
	}

	const entity::LightGameObject& SceneManager::getDirectionalLight() const
	{
		return m_directionalLight;
	}

	entity::LightGameObject& SceneManager::getWritableDirectionalLight()
	{
		return m_directionalLight;
	}

	const std::vector<entity::PointLightGameObject*>& SceneManager::getPointLights() const
	{
		return m_pointLights;
	}

	std::vector<entity::PointLightGameObject*>& SceneManager::getWritablePointLights()
	{
		return m_pointLights;
	}

	const std::vector<entity::SpotLightGameObject*>& SceneManager::getSpotLights() const
	{
		return m_spotLights;
	}

	std::vector<entity::SpotLightGameObject*>& SceneManager::getWritableSpotLights()
	{
		return m_spotLights;
	}

	const std::vector<entity::RenderableGameObject*>& SceneManager::getRenderables() const
	{
		return m_renderables;
	}

	std::vector<entity::RenderableGameObject*>& SceneManager::getWritableRenderables()
	{
		return m_renderables;
	}

	const entity::RenderableGameObject& SceneManager::getSkybox() const
	{
		return m_skybox;
	}

	entity::RenderableGameObject& SceneManager::getWritableSkybox()
	{
		return m_skybox;
	}

	const entity::RenderableGameObject& SceneManager::getOcean() const
	{
		return m_ocean;
	}

	entity::RenderableGameObject& SceneManager::getWritableOcean()
	{
		return m_ocean;
	}

	const entity::RenderableGameObject& SceneManager::getClouds() const
	{
		return m_clouds;
	}

	entity::RenderableGameObject& SceneManager::getWritableClouds()
	{
		return m_clouds;
	}

	const std::vector<entity::PhysicsGameObject*>& SceneManager::getPhysicsObjects() const
	{
		return m_physicsObjects;
	}

	std::vector<entity::PhysicsGameObject*>& SceneManager::getWritablePhysicsObjects()
	{
		return m_physicsObjects;
	}

	const std::vector<physics::Spring*>& SceneManager::getSprings() const
	{
		return m_springs;
	}

	std::vector<physics::Spring*>& SceneManager::getWritableSprings()
	{
		return m_springs;
	}

	const physics::ParticleSystem& SceneManager::getParticleSystem() const
	{
		return *m_particleSystem;
	}

	physics::ParticleSystem& SceneManager::getWritableParticleSystem()
	{
		return *m_particleSystem;
	}

	const entity::GameObject& SceneManager::getGameObject(const std::string& label) const
	{
		if (m_gameObjectMap.find(label) != m_gameObjectMap.end())
		{
			return *(m_gameObjectMap.at(label));
		}
	}

	void SceneManager::addGameObject(entity::GameObject* gameObject)
	{
		if (m_gameObjectMap.find(gameObject->getLabel()) == m_gameObjectMap.end())
		{
			m_gameObjectMap.insert({ gameObject->getLabel() , gameObject });

			if (auto* renderable = dynamic_cast<entity::RenderableGameObject*>(gameObject)) m_renderables.push_back(renderable);
			if (auto* physicsObject = dynamic_cast<entity::PhysicsGameObject*>(gameObject)) m_physicsObjects.push_back(physicsObject);
			if (auto* camera = dynamic_cast<entity::CameraGameObject*>(gameObject))         m_cameras.push_back(camera);

			if (auto* spotLight = dynamic_cast<entity::SpotLightGameObject*>(gameObject))   m_spotLights.push_back(spotLight);
			else if (auto* pointLight = dynamic_cast<entity::PointLightGameObject*>(gameObject)) m_pointLights.push_back(pointLight);
		}
		else
		{
			utils::ErrorLogger::log("Failed to add game object because an object with label '" + gameObject->getLabel() + "' already exists");
		}
	}

	void SceneManager::addObjectTrack(entity::GameObjectTrack* objectTrack)
	{
		m_objectTracks.insert(std::make_pair(objectTrack->getId(), objectTrack));
	}

	std::unordered_map<std::string, entity::GameObjectTrack*>& SceneManager::getObjectTracks()
	{
		return m_objectTracks;
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
			for (int i = 0; i < m_renderables.size(); ++i)
			{
				if (iterator->second == m_renderables[i])
				{
					m_renderables.erase(m_renderables.begin() + i);
				}
			}

			// Remove from physics
			for (int i = 0; i < m_physicsObjects.size(); ++i)
			{
				if (iterator->second == m_physicsObjects[i])
				{
					m_physicsObjects.erase(m_physicsObjects.begin() + i);
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

	const SceneConfig& SceneManager::getSceneConfig() const
	{
		return m_sceneConfig;
	}

	SceneConfig& SceneManager::getWritableSceneConfig()
	{
		return m_sceneConfig;
	}

	void SceneManager::mouseButtonDelegate(const input::MouseEvent& mouseEvent, float deltaTime)
	{
		if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftPress)
		{
			if (!(ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered()))
			{
				checkSelectingObject();
			}
		}
		if (mouseEvent.getType() == input::MouseEvent::EventType::eLeftRelease)
		{
			if (!(getAxisEditSubState() == AxisEditSubState::eEditNone))
			{
				stopAxisEdit();
			}
		}
	}

	void SceneManager::mouseScrollDelegate(const input::MouseEvent& mouseEvent, float deltaTime)
	{
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
	}

	void SceneManager::mouseMoveDelegate(const input::MouseEvent& mouseEvent, float deltaTime)
	{
		if (mouseEvent.getType() == input::MouseEvent::EventType::eRawMove)
		{
			if (InputManager::it().isRightMouseDown())
			{
				float moveFactor = m_activeCamera->getFOV() / 90.0f;
				if (moveFactor > 1.0f) moveFactor = 1.0f;
				m_activeCamera->getWritableTransform().rotateUsingAxis(m_activeCamera->getTransform().getRightVector(), (float)mouseEvent.getPos().y * 0.01f * moveFactor);
				m_activeCamera->getWritableTransform().rotateUsingAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (float)mouseEvent.getPos().x * 0.01f * moveFactor);
			}
		}
	}

	void SceneManager::checkSelectingObject()
	{
		float closestDist = FLT_MAX;
		float distance;

		if (objectIsSelected())
		{
			XMFLOAT3 objectPos = m_selectedObject->getTransform().getPositionFloat3();

			float objectHitRadius = m_selectedObject->getModel().getHitRadius();

			// Clamp object scale
			if (objectHitRadius < 0.75f) objectHitRadius = 0.75f;

			float camdist = XMVectorGetX(XMVector3Length(m_selectedObject->getTransform().getPositionVector() - m_activeCamera->getTransform().getPositionVector()));
			objectHitRadius *= camdist * 0.5f;

			// Translation
			if (m_axisEditState == AxisEditState::eEditTranslate)
			{
				// Update bounding box info
				for (int axisIndex = 0; axisIndex < 3; axisIndex++)
				{
					const XMFLOAT3& defaultBounds = m_axisTranslateDefaultBounds[axisIndex];
					m_axisTranslateBoudingBoxes[axisIndex].Extents = XMFLOAT3(defaultBounds.x * objectHitRadius, defaultBounds.y * objectHitRadius, defaultBounds.z * objectHitRadius);

					float xRadiusModifer = axisIndex == 0 ? 0.6f : 0.0f;
					float yRadiusModifer = axisIndex == 1 ? 0.6f : 0.0f;
					float zRadiusModifer = axisIndex == 2 ? 0.6f : 0.0f;

					m_axisTranslateBoudingBoxes[axisIndex].Center = XMFLOAT3(objectPos.x + objectHitRadius * xRadiusModifer, objectPos.y + objectHitRadius * yRadiusModifer, objectPos.z + objectHitRadius * zRadiusModifer);
				}

				// Check if an axis has been clicked on
				for (int axisIndex = 0; axisIndex < 3; axisIndex++)
				{
					if (m_axisTranslateBoudingBoxes[axisIndex].Intersects(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), distance))
					{
						if (distance < closestDist)
						{
							closestDist = distance;
							m_axisEditSubState = static_cast<AxisEditSubState>(axisIndex);
						}
					}
				}

				// Stop following track if an axis has been clicked on
				if (closestDist != FLT_MAX)
				{
					m_selectedObject->setFollowingObjectTrack(false);
					return;
				}
			}
			// Rotation
			else if (m_axisEditState == AxisEditState::eEditRotate)
			{
				XMMATRIX modelRotationMatrix = m_selectedObject->getTransform().getRotationMatrix();
				objectHitRadius *= 0.5f;
				{
					//GET TRANSFORMED AXIS VECTOR (PLANE NORMAL)
					XMVECTOR planeNormal = XMVector3Normalize(XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), modelRotationMatrix));
					//GET INTERSECT POINT WITH THIS PLANE AND THE MOUSE RAY
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector(); //FIND VECTOR DIFFERENCE
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));//GET SCALAR DISTANCE
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{ //CHECK IF THE INTERSECT IS ON THE RING
						XMVECTOR camToIntersect = planeIntersectPoint - m_activeCamera->getTransform().getPositionVector(); //FIND VECTOR DIFF FROM CAMERA TO THE POINT
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
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{
						XMVECTOR camToIntersect = planeIntersectPoint - m_activeCamera->getTransform().getPositionVector();
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
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
					if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f)
					{
						XMVECTOR camToIntersect = planeIntersectPoint - m_activeCamera->getTransform().getPositionVector();
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
			if (entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second))
			{
				distance = gameObject->getRayIntersectDist(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection());
				if (distance < closestDist)
				{
					closestDist = distance;
					m_selectedObject = gameObject;
				}
			}

			mapIterator++;
		}

		// If an object was not clicked, deselect the current
		if (closestDist == FLT_MAX)
		{
			m_selectedObject = nullptr;
			m_axisEditSubState = AxisEditSubState::eEditNone;
			m_lastAxisGrabOffset = FLT_MAX;
		}
	}

	void SceneManager::updateSelectedObject()
	{
		// Check if mouse is on screen
		const input::MousePosNDC& mousePosNDC = InputManager::it().getMousePosNDC();
		if (mousePosNDC.x > -1.0f && mousePosNDC.x < 1.0f && mousePosNDC.y > -1.0f && mousePosNDC.y < 1.0f)
		{
			// Translating
			if (m_axisEditState == AxisEditState::eEditTranslate)
			{
				XMFLOAT3 objectPos = m_selectedObject->getTransform().getPositionFloat3();
				switch (m_axisEditSubState)
				{
				case AxisEditSubState::eEditX:
				{
					//GET INTERSECT POINT WITH MOUSE RAY
					XMVECTOR intersect = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetX(intersect); //GET RELEVANT VECTOR COMPONENT
					if (m_lastAxisGrabOffset != FLT_MAX)
					{ //CHECK IF FIRST ITERATION
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset; //FIND DIFFERENCE
						m_selectedObject->getWritableTransform().adjustPosition(diff, 0.0f, 0.0f); //MOVE OBJECT BY DIFFERENCE
					}
					m_lastAxisGrabOffset = currentAxisGrabOffset; //SET LAST OFFSET
					break;
				}
				case AxisEditSubState::eEditY:
				{
					XMVECTOR intersect = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), XMVector3Normalize(XMVectorSet(m_activeCamera->getTransform().getPositionFloat3().x - objectPos.x, 0.0f, m_activeCamera->getTransform().getPositionFloat3().z - objectPos.z, 0.0f)), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetY(intersect);
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset;
						m_selectedObject->getWritableTransform().adjustPosition(0.0f, diff, 0.0f);
					}
					m_lastAxisGrabOffset = currentAxisGrabOffset;
					break;
				}
				case AxisEditSubState::eEditZ:
				{
					XMVECTOR intersect = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					float currentAxisGrabOffset = XMVectorGetZ(intersect);
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float diff = currentAxisGrabOffset - m_lastAxisGrabOffset;
						m_selectedObject->getWritableTransform().adjustPosition(0.0f, 0.0f, diff);
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
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector(); //COMPUTE DIFFERENCE
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix); //TRANSFORM TO MODEL SPACE
					float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetZ(modelSpaceCentreDiff)); //WORK OUT ANGLE OF ROTATION
					if (m_lastAxisGrabOffset != FLT_MAX)
					{ //IF NOT FIRST ITERATION
						float rotationDiff = rotation - m_lastAxisGrabOffset; //FIND ANGLE DIFF
						m_selectedObject->getWritableTransform().rotateUsingAxis(m_selectedObject->getTransform().getRightVector(), -rotationDiff); //ROTATE AXIS BY DIFFERENCE
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
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
					float rotation = atan2(XMVectorGetZ(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float rotationDiff = rotation - m_lastAxisGrabOffset;
						m_selectedObject->getWritableTransform().rotateUsingAxis(m_selectedObject->getTransform().getUpVector(), -rotationDiff);
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
					XMVECTOR planeIntersectPoint = physics::collision::rayPlaneIntersect(m_activeCamera->getTransform().getPositionVector(), m_activeCamera->getMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
					XMVECTOR centreDiff = planeIntersectPoint - m_selectedObject->getTransform().getPositionVector();
					XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
					float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
					if (m_lastAxisGrabOffset != FLT_MAX)
					{
						float rotationDiff = rotation - m_lastAxisGrabOffset;
						m_selectedObject->getWritableTransform().rotateUsingAxis(m_selectedObject->getTransform().getFrontVector(), rotationDiff);
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

	bool SceneManager::objectIsSelected() const
	{
		return m_selectedObject != nullptr;
	}

	const entity::RenderableGameObject* SceneManager::getSelectedObject() const
	{
		return m_selectedObject;
	}

	entity::RenderableGameObject* SceneManager::getWritableSelectedObject()
	{
		return m_selectedObject;
	}

	void SceneManager::setSelectedObject(entity::RenderableGameObject* object)
	{
		m_selectedObject = object;
	}

	float SceneManager::getWaterHeightAt(float posX, float posZ, bool exact) const
	{
		float value = 0.0f;
		value += sin(-posX * 0.4f + m_gameTime * 1.2f) * 0.15f + sin(posZ * 0.5f + m_gameTime * 1.3f) * 0.15f;
		value += sin(posX * 0.2f + m_gameTime * 0.6f) * 0.5f + sin(-posZ * 0.22f + m_gameTime * 0.4f) * 0.45f;
		if (exact)
		{
			value += sin(posX * 1.5f + m_gameTime * 0.0017f) * 0.05f + sin(posZ * 1.5f + m_gameTime * 0.0019f) * 0.05f;
		}
		return value; // * amplitude
	}

	XMVECTOR SceneManager::getFourierOffset(float x, float z) const
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

		XMVECTOR flatPosition = XMVectorSet(x, 0.0f, z, 1.0f);
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

	float SceneManager::getGameTime() const
	{
		return m_gameTime;
	}

	float SceneManager::getDayProgress() const
	{
		return m_dayProgress;
	}

	float* SceneManager::getDayProgressPtr()
	{
		return &m_dayProgress;
	}

	bool SceneManager::isUsingDayNightCycle() const
	{
		return m_dayNightCycle;
	}

	bool* SceneManager::getDayNightCyclePtr()
	{
		return &m_dayNightCycle;
	}

	bool SceneManager::isPaused() const
	{
		return m_paused;
	}

	bool* SceneManager::getPausedPtr()
	{
		return &m_paused;
	}

	void SceneManager::floatObject(entity::GameObject* object) const
	{
		const XMFLOAT3& anchorPosition = object->getTransform().getPositionFloat3();

		XMVECTOR objectFront = object->getTransform().getFrontVector();
		XMVECTOR objectRight = object->getTransform().getRightVector();

		//XMVECTOR flatTangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectRight), 0.0f, XMVectorGetZ(objectRight), 0.0f));
		//XMVECTOR flatBitangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectFront), 0.0f, XMVectorGetZ(objectFront), 0.0f));

		XMVECTOR displacedMainPosition = getFourierOffset(anchorPosition.x, anchorPosition.z);
		//XMVECTOR displacedTangentPosition = getFourierOffset(anchorPosition.x + XMVectorGetX(flatTangent), anchorPosition.z + XMVectorGetZ(flatTangent));
		//XMVECTOR displacedBitangentPosition = getFourierOffset(anchorPosition.x + XMVectorGetX(flatBitangent), anchorPosition.z + XMVectorGetZ(flatBitangent));
		XMVECTOR displacedTangentPosition = getFourierOffset(anchorPosition.x + 4.0f, anchorPosition.z);
		XMVECTOR displacedBitangentPosition = getFourierOffset(anchorPosition.x , anchorPosition.z - 4.0f);

		XMStoreFloat3(&(object->getWritableFloatOffset()), displacedMainPosition);

		XMVECTOR tangent = XMVector3Normalize(displacedTangentPosition - displacedMainPosition);
		XMVECTOR bitangent = XMVector3Normalize(displacedBitangentPosition - displacedMainPosition);
		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(tangent, bitangent));

		/*XMVECTOR tangent = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR bitangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);*/

		bitangent = XMVector3Normalize(XMVector3Cross(tangent, normal));

		XMMATRIX rotationMatrix = XMMATRIX(tangent, normal, bitangent, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		rotationMatrix.r[0].m128_f32[3] = 0.0f;
		rotationMatrix.r[1].m128_f32[3] = 0.0f;
		rotationMatrix.r[2].m128_f32[3] = 0.0f;

		object->getWritableTransform().setOrientationRotationMatrix(rotationMatrix);
	}

	AxisEditState SceneManager::getAxisEditState() const
	{
		return m_axisEditState;
	}

	void SceneManager::setAxisEditState(AxisEditState axisEditState)
	{
		m_axisEditState = axisEditState;
	}

	AxisEditSubState SceneManager::getAxisEditSubState() const
	{
		return m_axisEditSubState;
	}

	void SceneManager::setAxisEditSubState(AxisEditSubState axisEditSubState)
	{
		m_axisEditSubState = axisEditSubState;
	}

	void SceneManager::stopAxisEdit()
	{
		m_axisEditSubState = AxisEditSubState::eEditNone;
		m_lastAxisGrabOffset = FLT_MAX;
	}

	entity::ControllerManager* SceneManager::getWritableControllerManager()
	{
		return m_controllerManager;
	}

}
