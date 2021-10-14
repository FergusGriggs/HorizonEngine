
//Function implementations for the Graphics class

#include "graphics_handler.h"

namespace hrzn::gfx
{
	bool GraphicsHandler::initialize(HWND hwnd, int width, int height, entity::ControllerManager* controllerManager)
	{
		m_fpsTimer.start();

		m_windowWidth = width;
		m_windowHeight = height;

		m_controllerManager = controllerManager;

		if (!initializeDirectX(hwnd))
		{
			return false;
		}

		if (!ResourceManager::it().initialize(m_device.Get(), m_deviceContext.Get()))
		{
			return false;
		}

		if (!initializeShaders())
		{
			return false;
		}

		//INIT SCENE OBJECTS
		if (!initializeScene())
		{
			return false;
		}

		//INIT IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());
		ImGui::StyleColorsDark();

		return true;
	}

	bool GraphicsHandler::initializeShaders()
	{
		std::wstring shaderFolder = L"";
	#pragma region DetermineShaderPath
		//if (IsDebuggerPresent() == TRUE)
		//{
	#ifdef _DEBUG //Debug Mode
	#ifdef _WIN64 //x64
			shaderFolder = L"../x64/Debug/";
	#else  //x86 (Win32)
			shaderFolder = L"../Debug/";
	#endif
	#else //Release Mode
	#ifdef _WIN64 //x64
			//shaderFolder = L"../x64/Release/";
		shaderFolder = L"res/shaders/compiled/";
	#else  //x86 (Win32)
			shaderFolder = L"../Release/";
	#endif
	#endif
		//}

		//Create vertex shader input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		UINT numElements = ARRAYSIZE(layout);

		//Initialise vertex shaders
		if (!m_vertexShader.initialize(m_device, shaderFolder + L"vertexShader.cso", layout, numElements))
		{
			return false;
		}

		if (!m_waterVertexShader.initialize(m_device, shaderFolder + L"waterVertexShader.cso", layout, numElements))
		{
			return false;
		}

		//Initialise pixel shaders
		if (!m_pixelShader.initialize(m_device, shaderFolder + L"pixelShader.cso"))
		{
			return false;
		}

		if (!m_noLightPixelShader.initialize(m_device, shaderFolder + L"noLightPixelShader.cso"))
		{
			return false;
		}

		if (!m_atmosphericPixelShader.initialize(m_device, shaderFolder + L"atmosphericPixelShader.cso"))
		{
			return false;
		}

		if (!m_cloudsPixelShader.initialize(m_device, shaderFolder + L"cloudsPixelShader.cso"))
		{
			return false;
		}

		if (!m_waterPixelShader.initialize(m_device, shaderFolder + L"waterPixelShader.cso"))
		{
			return false;
		}

		// Initialise compute shaders
		if (!m_noiseTextureComputeShader.initialize(m_device, shaderFolder + L"noiseTextureComputeShader.cso"))
		{
			return false;
		}

		// Initialise global shader vars
		HRESULT hr = m_noiseTextureComputeShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create 'noiseTextureComputeShader' constant buffer.");
		create3DNoiseTexture();
	}

	bool GraphicsHandler::initializeScene()
	{
		try
		{
			//CREATE CONSTANT BUFFERS
			HRESULT hr = m_vertexShaderCB.initialize(m_device.Get(), m_deviceContext.Get());

			hr = m_waterVertexShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'waterVertexShader' constant buffer.");
			m_waterVertexShaderCB.m_data.m_waveCount = 20;
			m_waterVertexShaderCB.m_data.m_waveScale = 9.3f;
			m_waterVertexShaderCB.m_data.m_wavePeriod = 26.5f;
			m_waterVertexShaderCB.m_data.m_waveSpeed = 21.0f;
			m_waterVertexShaderCB.m_data.m_waveSeed = 258.671f;
			m_waterVertexShaderCB.m_data.m_waveScaleMultiplier = 0.538f;

			hr = m_waterPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'waterPixelShader' constant buffer.");
			m_waterPixelShaderCB.m_data.m_waveCount = m_waterVertexShaderCB.m_data.m_waveCount;
			m_waterPixelShaderCB.m_data.m_waveScale = m_waterVertexShaderCB.m_data.m_waveScale;
			m_waterPixelShaderCB.m_data.m_wavePeriod = m_waterVertexShaderCB.m_data.m_wavePeriod;
			m_waterPixelShaderCB.m_data.m_waveSpeed = m_waterVertexShaderCB.m_data.m_waveSpeed;
			m_waterPixelShaderCB.m_data.m_waveSeed = m_waterVertexShaderCB.m_data.m_waveSeed;
			m_waterPixelShaderCB.m_data.m_waveScaleMultiplier = m_waterVertexShaderCB.m_data.m_waveScaleMultiplier;
			m_waterPixelShaderCB.m_data.m_foamStart = 1.446f;
			m_waterPixelShaderCB.m_data.m_colourChangeStart = 1.0f;

			hr = m_pixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'pixelShader' constant buffer.");
			m_pixelShaderCB.m_data.m_useNormalMapping = true;

			hr = m_noLightPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'noLightPixelShader' constant buffer.");

			hr = m_atmosphericPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'noLightPixelShader' constant buffer.");
			XMStoreFloat3(&m_atmosphericPixelShaderCB.m_data.m_sunDirection, XMVector3Normalize(XMVectorSet(0.8f, 0.5f, 0.4f, 1.0f)));
			m_atmosphericPixelShaderCB.m_data.m_sunSize = 75.0f;
			m_atmosphericPixelShaderCB.m_data.m_density = 0.65f;
			m_atmosphericPixelShaderCB.m_data.m_multiScatterPhase = 0.27f;
			m_atmosphericPixelShaderCB.m_data.m_anisotropicIntensity = 1.0f;
			m_atmosphericPixelShaderCB.m_data.m_zenithOffset = -0.02f;
			m_atmosphericPixelShaderCB.m_data.m_nightDensity = 1.2f;
			m_atmosphericPixelShaderCB.m_data.m_nightZenithYClamp = 0.1f;

			hr = m_cloudsPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'cloudsPixelShader' constant buffer."); // Fluffy 1 // Fluffy 2 // Bulky bois
			m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionThroughClouds = 0.084f; //             0.084f      0.084f      0.338f
			m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionTowardsSun = 0.392f; //                0.273f      0.392f      0.559f
			m_cloudsPixelShaderCB.m_data.m_phaseFactor = 0.266f; //                              0.208f      0.266f      0.428f
			m_cloudsPixelShaderCB.m_data.m_darknessThreshold = 0.073f; //                        0.09f       0.073f      0.09f
			m_cloudsPixelShaderCB.m_data.m_cloudCoverage = 0.497f; //                            0.465f      0.497f      0.446f
			m_cloudsPixelShaderCB.m_data.m_cloudSpeed = 0.02f; // 
			m_cloudsPixelShaderCB.m_data.m_numSteps = 40; //
			m_cloudsPixelShaderCB.m_data.m_stepSize = 85.0f; //
			m_cloudsPixelShaderCB.m_data.m_cloudHeight = 2000.0f;

			//LOAD AXIS MODELS
			if (!m_axisTranslateModel.initialize("res/models/axis/translate2.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}

			if (!m_axisRotateModel.initialize("res/models/axis/rotate2.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}

			if (!m_particleMesh.initialize("res/models/particle.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}

			if (!m_springModel.initialize("res/models/spring.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}

			std::string defaultDiffuseTextureFilePath = "res/textures/scales/diffuse.jpg";
			std::string defaultSpecularTextureFilePath = "res/textures/scales/specular.jpg";
			std::string defaultNormalTextureFilePath = "res/textures/scales/normal.jpg";

			m_defaultDiffuseTexture = new Texture(m_device.Get(), defaultDiffuseTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);
			m_defaultSpecularTexture = new Texture(m_device.Get(), defaultSpecularTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);
			m_defaultNormalTexture = new Texture(m_device.Get(), defaultNormalTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);

			std::string highlightDiffuseTextureFilePath = "res/textures/engraved/diffuse.jpg";
			std::string highlightSpecularTextureFilePath = "res/textures/engraved/specular.jpg";
			std::string highlightNormalTextureFilePath = "res/textures/engraved/normal.jpg";

			m_highlightDiffuseTexture = new Texture(m_device.Get(), highlightDiffuseTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);
			m_highlightSpecularTexture = new Texture(m_device.Get(), highlightSpecularTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);
			m_highlightNormalTexture = new Texture(m_device.Get(), highlightNormalTextureFilePath, aiTextureType::aiTextureType_DIFFUSE);

			m_particleSystem = new entity::physics::ParticleSystem(&m_particleMesh);
			m_particleSystem->addEmitter(XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.25f, 5.0f, 0.25f, 1.5f, 0.25f, 0.005f, 0.25f);

			//SET TRANSLATE AXIS DEFAULT BOUNDS / EXTENTS
			m_xAxisTranslateDefaultBounds = XMFLOAT3(0.45f, 0.05f, 0.05f);
			m_yAxisTranslateDefaultBounds = XMFLOAT3(0.05f, 0.45f, 0.05f);
			m_zAxisTranslateDefaultBounds = XMFLOAT3(0.05f, 0.05f, 0.45f);

			if (!m_skybox.initialize("Skybox", "res/models/skyboxes/ocean.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}

			if (!m_clouds.initialize("Clouds", "res/models/simple_plane.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}
			m_clouds.getTransform().setPosition(0.0f, 2000.0f, 0.0f);
			m_clouds.setScale(XMFLOAT3(200.0f, 200.0f, 200.0f));

			if (!m_ocean.initialize("Ocean", "res/models/ocean_tesselated.obj", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}
			m_ocean.getTransform().setPosition(0.0f, -15.0f, 0.0f);

			if (!m_directionalLight.initialize("Directional Light", m_device.Get(), m_deviceContext.Get()))
			{
				return false;
			}
			m_directionalLight.getTransform().setPosition(2.0f, 6.0f, 2.0f);
			m_directionalLight.getTransform().lookAtPosition(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
			m_directionalLight.setColour(XMFLOAT3(0.9f, 0.85f, 0.8f));

			if (!loadScene("city.txt"))
			{
				return false;
			}

			//dynamic_cast<RenderableGameObject*>(gameObjectMap.at("floor1"))->SetScale(XMFLOAT3(5.0f, 1.0f, 5.0f));

			//springs.push_back(new Spring(XMVectorSet(0.0f, 10.0f, 5.0f, 0.0f), physicsGameObjects.at(0)->GetRigidBody(), 5.0f, 150.0f));
			//springs.push_back(new Spring(physicsGameObjects.at(3)->GetRigidBody(), physicsGameObjects.at(4)->GetRigidBody(), 10.0f, 150.0f));

			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box1"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box2"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box3"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box4"))->GetRigidBody()->SetIsStatic(false);
			//dynamic_cast<PhysicsGameObject*>(gameObjectMap.at("box5"))->GetRigidBody()->SetIsStatic(false);

			m_camera.getTransform().setPosition(0.0f, 10.0f, -7.0f);
			m_camera.getTransform().lookAtPosition(XMVectorSet(0.0f, 7.0f, 0.0f, 1.0f));
			m_camera.setProjectionValues(90.0f, static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight), 0.1f, 1000.0f);
			//camera.SetObjectTrack(objectTracks.at("camera_track"));
			//camera.SetFollowingObjectTrack(true);
		}
		catch (utils::COMException & exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}
		return true;
	}

	void GraphicsHandler::renderFrame(float deltaTime)
	{
		float blackColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), blackColour);

		m_camera.updateView();

		//Directional light shader variables
		m_directionalLight.updateShaderVariables(m_pixelShaderCB);

		//Point light shader variables
		size_t numPointLights = m_pointLights.size();
		for (size_t i = 0; i < numPointLights; ++i)
		{
			m_pointLights.at(i)->updateShaderVariables(m_pixelShaderCB, i);
		}
	
		//Spot light shader variables
		size_t numSpotLights = m_spotLights.size();
		for (size_t i = 0; i < numSpotLights; ++i)
		{
			m_spotLights.at(i)->updateShaderVariables(m_pixelShaderCB, i);
		}

		//General shader variables
		m_pixelShaderCB.m_data.m_numPointLights = numPointLights;
		m_pixelShaderCB.m_data.m_numSpotLights = numSpotLights;

		m_pixelShaderCB.m_data.m_cameraPosition = m_camera.getTransform().getPositionFloat3();

		m_pixelShaderCB.m_data.m_objectMaterial.m_shininess = 4.0f;
		m_pixelShaderCB.m_data.m_objectMaterial.m_specularity = 0.75f;

		m_pixelShaderCB.mapToGPU();
		m_deviceContext->PSSetConstantBuffers(0, 1, m_pixelShaderCB.getAddressOf());

		//CLEAR RENDER TARGET VIEW AND DEPTH STENCIL VIEW
		float backgroundColour[] = { 0.62f * m_directionalLight.m_colour.x, 0.90 * m_directionalLight.m_colour.y, 1.0f * m_directionalLight.m_colour.z, 1.0f };
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), backgroundColour);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//UPDATE INPUT ASSEMBLER
		m_deviceContext->IASetInputLayout(m_vertexShader.getInputLayout());
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SET RASTERIZER STATE
		if (m_useWireframe)
		{
			m_deviceContext->RSSetState(m_wireframeRasterizerState.Get());
		}
		else
		{
			m_deviceContext->RSSetState(m_regularRasterizerState.Get());
		}

		//SET DEPTH STENCIL STATE
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		//SET BLEND STATE
		m_deviceContext->OMSetBlendState(m_blendState.Get(), NULL, 0xFFFFFFFF);//blendState.Get()

		//SET SAMPLER STATE
		m_deviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		//SET VERTEX AND PIXEL SHADERS
		m_deviceContext->VSSetShader(m_vertexShader.getShader(), NULL, 0);
	
		UINT offset = 0;

		XMMATRIX viewProjMat = m_camera.getViewMatrix() * m_camera.getProjectionMatrix();

		{
			//DRAW SKYBOX

			m_deviceContext->PSSetShader(m_atmosphericPixelShader.getShader(), NULL, 0);
			m_deviceContext->PSSetConstantBuffers(0, 1, m_atmosphericPixelShaderCB.getAddressOf());

			m_atmosphericPixelShaderCB.m_data.m_cameraPosition = m_camera.getTransform().getPositionFloat3();

			if (m_dayNightCycle)
			{
				m_dayProgress += deltaTime * 0.0166666; // 1 day = 60 seconds
				m_gameTime += deltaTime;

				m_vertexShaderCB.m_data.m_gameTime = m_gameTime;
				m_waterVertexShaderCB.m_data.m_gameTime = m_gameTime;
				m_cloudsPixelShaderCB.m_data.m_gameTime = m_gameTime;
				m_waterPixelShaderCB.m_data.m_gameTime = m_gameTime;
			}

			if (m_dayProgress > 1.0f) m_dayProgress -= 1.0f;

			float dayOrNight = 0.0f;
			float zeroToOneDayOrNight = modf(m_dayProgress * 2.0f, &dayOrNight);
			float split = 1.0f - abs(zeroToOneDayOrNight - 0.5f) * 2.0f;

			if (dayOrNight == 0.0f)
			{
				float t = fmaxf(0.0f, fminf(1.0f, (split - 0.25f) / (0.1f - 0.25f)));
				float densityMod = t * t * (3.0f - 2.0f * t);
				m_atmosphericPixelShaderCB.m_data.m_density = 0.142f + densityMod * (0.65f - 0.142f);

				XMFLOAT3 sunsetColour = XMFLOAT3(1.0f, 0.62f, 0.26f);
				XMFLOAT3 daySunColour = XMFLOAT3(1.0f, 0.85f, 0.65f);

				XMFLOAT3 sunColour = XMFLOAT3(0.0f, 0.0f, 0.0f);
				XMStoreFloat3(&sunColour, XMVectorLerp(XMLoadFloat3(&daySunColour), XMLoadFloat3(&sunsetColour), t));
				m_directionalLight.setColour(sunColour);
			}
			else
			{
				float density = fmaxf(0.0f, fminf(1.0f, (split - 0.35f) / (0.0f - 0.35f)));

				float lightLerp = fmaxf(0.0f, fminf(1.0f, (split - 0.25f) / (0.0f - 0.25f)));
				float smoothLightLerp = lightLerp * lightLerp * (3.0f - 2.0f * lightLerp);

				XMFLOAT3 sunsetColour = XMFLOAT3(1.0f, 0.62f, 0.26f);
				XMFLOAT3 nightSunColour = XMFLOAT3(0.025f, 0.025f, 0.0375f);

				XMFLOAT3 sunColour = XMFLOAT3(0.0f, 0.0f, 0.0f);
				XMStoreFloat3(&sunColour, XMVectorLerp(XMLoadFloat3(&nightSunColour), XMLoadFloat3(&sunsetColour), smoothLightLerp));
				m_directionalLight.setColour(sunColour);

				m_atmosphericPixelShaderCB.m_data.m_density = 0.25f + density * (0.65f - 0.25f);
			}

			XMVECTOR sunDirection = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationAxis(XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), m_dayProgress * sc_2PI));
			m_directionalLight.getTransform().setPosition(sunDirection);
			m_directionalLight.getTransform().lookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

			XMStoreFloat3(&m_sunDirection, sunDirection);

			m_atmosphericPixelShaderCB.m_data.m_sunDirection = m_sunDirection;
			
			m_atmosphericPixelShaderCB.mapToGPU();

			m_skybox.getTransform().setPosition(m_camera.getTransform().getPositionFloat3());
			m_skybox.draw(viewProjMat, &m_vertexShaderCB);

			m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		m_deviceContext->PSSetConstantBuffers(0, 1, m_pixelShaderCB.getAddressOf());
		m_deviceContext->PSSetShader(m_pixelShader.getShader(), NULL, 0);

		{
			m_deviceContext->PSSetShaderResources(0, 1, m_defaultDiffuseTexture->getTextureResourceViewAddress());
			m_deviceContext->PSSetShaderResources(1, 1, m_defaultSpecularTexture->getTextureResourceViewAddress());
			m_deviceContext->PSSetShaderResources(2, 1, m_defaultNormalTexture->getTextureResourceViewAddress());

			//DRAW REGULAR GAME OBJECTS
			size_t numRenderableGameObjects = m_renderableGameObjects.size();
			for (size_t i = 0; i < numRenderableGameObjects; ++i)
			{
				entity::RenderableGameObject* renderableGameObject = m_renderableGameObjects.at(i);

				if (renderableGameObject->getFloating())
				{
					floatObject(renderableGameObject);
				}

				if (renderableGameObject == m_selectedObject)
				{
					m_deviceContext->PSSetShaderResources(0, 1, m_highlightDiffuseTexture->getTextureResourceViewAddress());
					m_deviceContext->PSSetShaderResources(1, 1, m_highlightSpecularTexture->getTextureResourceViewAddress());
					m_deviceContext->PSSetShaderResources(2, 1, m_highlightNormalTexture->getTextureResourceViewAddress());

					renderableGameObject->draw(viewProjMat, &m_vertexShaderCB, false);

					m_deviceContext->PSSetShaderResources(0, 1, m_defaultDiffuseTexture->getTextureResourceViewAddress());
					m_deviceContext->PSSetShaderResources(1, 1, m_defaultSpecularTexture->getTextureResourceViewAddress());
					m_deviceContext->PSSetShaderResources(2, 1, m_defaultNormalTexture->getTextureResourceViewAddress());
				}
				else
				{
					renderableGameObject->draw(viewProjMat, &m_vertexShaderCB, false);
				}
			}

			//Draw particles
			//particleSystem->DrawParticles(viewProjMat, &cb_vs_vertexShader);


			//Draw Springs
			XMMATRIX springModelMatrix;
			for (int i = 0; i < m_springs.size(); ++i)
			{
				XMVECTOR springStart = m_springs[i]->getSpringStart()->getTransformReference()->getPositionVector();
				XMVECTOR springEnd = m_springs[i]->getSpringEnd()->getTransformReference()->getPositionVector();

				float scale = XMVectorGetX(XMVector3Length(springEnd - springStart)) / 5.0f;

				XMVECTOR front = XMVectorSetW(XMVector3Normalize(springEnd - springStart), 0.0f);
				XMVECTOR up = XMVectorSetW(XMVector3Normalize(XMVector3Cross(front, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))), 0.0f);
				XMVECTOR right = XMVectorSetW(XMVector3Cross(front, up), 0.0f);

				if (abs(XMVectorGetZ(front)) == 1.0f)
				{
					springModelMatrix = XMMatrixScaling(1.0f, 1.0f, scale) * XMMatrixTranslation(XMVectorGetX(springStart), XMVectorGetY(springStart), XMVectorGetZ(springStart));
				}
				else
				{
					springModelMatrix = XMMatrixScaling(1.0f, 1.0f, scale) * XMMATRIX(up, right, front, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)) * XMMatrixTranslation(XMVectorGetX(springStart), XMVectorGetY(springStart), XMVectorGetZ(springStart));
					//springModelMatrix = XMMatrixLookToLH(XMVectorZero(), front, up) * XMMatrixTranslation(XMVectorGetX(springStart), XMVectorGetY(springStart), XMVectorGetZ(springStart)); //XMMatrixScaling(1.0f, scale, 1.0f) * XMMatrixTranslation(XMVectorGetX(springStart), XMVectorGetY(springStart), XMVectorGetZ(springStart)) * 
				}
			
				//springModelMatrix = XMMatrixTranslation(XMVectorGetX(springStart), XMVectorGetY(springStart), XMVectorGetZ(springStart));

				m_springModel.draw(springModelMatrix, viewProjMat, &m_vertexShaderCB);
			}

			//DRAW WATER
			m_deviceContext->VSSetShader(m_waterVertexShader.getShader(), NULL, 0);
			m_deviceContext->PSSetShader(m_waterPixelShader.getShader(), NULL, 0);

			m_waterVertexShaderCB.mapToGPU();
			
			XMFLOAT3 cameraPosFloat = m_camera.getTransform().getPositionFloat3();
			m_ocean.getTransform().setPosition(cameraPosFloat.x, m_ocean.getTransform().getPositionFloat3().y, cameraPosFloat.z);

			m_waterPixelShaderCB.m_data.m_cameraPosition = cameraPosFloat;
			XMStoreFloat3(&m_waterPixelShaderCB.m_data.m_lightDirection, m_directionalLight.getTransform().getFrontVector());

			m_waterPixelShaderCB.mapToGPU();

			m_deviceContext->PSSetConstantBuffers(0, 1, m_waterPixelShaderCB.getAddressOf());

			m_ocean.draw(viewProjMat, &m_waterVertexShaderCB);

			//DRAWCLOUDS
			m_deviceContext->VSSetShader(m_vertexShader.getShader(), NULL, 0);
			m_deviceContext->PSSetShader(m_cloudsPixelShader.getShader(), NULL, 0);

			m_cloudsPixelShaderCB.m_data.m_cameraPosition = cameraPosFloat;

			XMStoreFloat3(&m_cloudsPixelShaderCB.m_data.m_lightDirection, m_directionalLight.getTransform().getFrontVector());
			m_cloudsPixelShaderCB.mapToGPU();

			m_deviceContext->PSSetConstantBuffers(0, 1, m_cloudsPixelShaderCB.getAddressOf());
			m_deviceContext->PSSetShaderResources(0, 1, m_noiseTextureShaderResourceView.GetAddressOf());

			m_clouds.getTransform().setPosition(cameraPosFloat.x, m_clouds.getTransform().getPositionFloat3().y, cameraPosFloat.z);

			m_clouds.draw(viewProjMat, &m_vertexShaderCB, false);
		}
	
		{
			//DRAW NO LIGHT OBJECTS
			m_deviceContext->PSSetShader(m_noLightPixelShader.getShader(), NULL, 0);

			m_deviceContext->PSSetConstantBuffers(0, 1, m_noLightPixelShaderCB.getAddressOf());

			for (size_t i = 0; i < numPointLights; ++i)
			{
				entity::PointLightGameObject* pointLight = m_pointLights.at(i);
				m_noLightPixelShaderCB.m_data.m_colour = pointLight->m_colour;
				m_noLightPixelShaderCB.mapToGPU();
				pointLight->draw(viewProjMat, &m_vertexShaderCB);
			}

			size_t numSpotLights = m_spotLights.size();
			for (size_t i = 0; i < numSpotLights; ++i)
			{
				entity::SpotLightGameObject* spotLight = m_spotLights.at(i);
				m_noLightPixelShaderCB.m_data.m_colour = spotLight->m_colour;
				m_noLightPixelShaderCB.mapToGPU();
				spotLight->draw(viewProjMat, &m_vertexShaderCB);
			}

			m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			if (m_selectingGameObject)
			{
				m_noLightPixelShaderCB.m_data.m_colour = XMFLOAT3(1.0f, 1.0f, 1.0f);
				m_noLightPixelShaderCB.mapToGPU();
				drawAxisForObject(m_selectedObject, viewProjMat);
			}
		}

		//UPDATE FPS TIMER
		m_fpsCounter++;

		if (m_fpsTimer.getMicrosecondsElapsed() > 1000000)
		{
			m_fpsString = "FPS: " + std::to_string(m_fpsCounter);
			m_fpsCounter = 0;
			m_fpsTimer.restart();
		}

		//RENDER UI (NOT IMGUI)
		m_spriteBatch->Begin();

		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(1.0f, 0.5f), DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	
		if (m_selectingGameObject)
		{
			// Stop text being drawn behind camera
			if (XMVectorGetX(XMVector3Dot(m_selectedObject->getTransform().getPositionVector() - m_camera.getTransform().getPositionVector(), m_camera.getTransform().getFrontVector())) >= 0.0f)
			{
				XMFLOAT2 screenNDC = m_camera.getNDCFrom3DPos(m_selectedObject->getTransform().getPositionVector() + XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f));//XMFLOAT2(-0.5,0.5);
				XMFLOAT2 screenPos = DirectX::XMFLOAT2((screenNDC.x * 0.5f + 0.5f) * m_windowWidth, (1.0f - (screenNDC.y * 0.5f + 0.5f)) * m_windowHeight);
				XMVECTOR size = m_spriteFont->MeasureString(utils::string_helpers::stringToWide(m_selectedObject->getLabel()).c_str());
				screenPos.x -= XMVectorGetX(size) * 0.5f;
				screenPos.y -= XMVectorGetY(size) * 0.5f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_selectedObject->getLabel()).c_str(), screenPos, DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
				screenPos.y -= 0.5f;
				screenPos.x -= 1.0f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_selectedObject->getLabel()).c_str(), screenPos, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			}
		}

		m_spriteBatch->End();

		ImGui::Render();

		//RENDER IMGUI
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		//PRESENT IMAGE
		m_swapChain->Present(m_useVSync, NULL);// using vsync
	}

	void GraphicsHandler::updateSelectedObject()
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

	XMFLOAT3 GraphicsHandler::readFloat3(std::ifstream& stream)
	{
		XMFLOAT3 vector;
		stream >> vector.x;
		stream >> vector.y;
		stream >> vector.z;
		return vector;
	}

	XMFLOAT4 GraphicsHandler::readFloat4(std::ifstream& stream)
	{
		XMFLOAT4 vector;
		stream >> vector.x;
		stream >> vector.y;
		stream >> vector.z;
		stream >> vector.w;
		return vector;
	}

	void GraphicsHandler::writeFloat3(const XMFLOAT3& float3, std::ofstream& stream)
	{
		stream << float3.x << ' ' << float3.y << ' ' << float3.z << ' ';
	}

	void GraphicsHandler::writeFloat3(const XMVECTOR& float3, std::ofstream& stream)
	{
		stream << XMVectorGetX(float3) << ' ' << XMVectorGetY(float3) << ' ' << XMVectorGetZ(float3) << ' ';
	}

	void GraphicsHandler::writeFloat4(const XMFLOAT4& float4, std::ofstream& stream)
	{
		stream << float4.x << ' ' << float4.y << ' ' << float4.z << ' ' << float4.w << ' ';
	}

	void GraphicsHandler::writeFloat4(const XMVECTOR& float4, std::ofstream& stream)
	{
		stream << XMVectorGetX(float4) << ' ' << XMVectorGetY(float4) << ' ' << XMVectorGetZ(float4) << ' ' << XMVectorGetW(float4) << ' ';
	}

	void GraphicsHandler::create3DNoiseTexture()
	{
		int size = 32;
		int height = 32;

		// Set up the constant buffer
		m_deviceContext->CSSetShader(m_noiseTextureComputeShader.getShader(), nullptr, 0);

		m_noiseTextureComputeShaderCB.m_data.m_size = static_cast<float>(size);
		m_noiseTextureComputeShaderCB.m_data.m_height = static_cast<float>(height);
		m_noiseTextureComputeShaderCB.m_data.m_seed = 0.0f;
		m_noiseTextureComputeShaderCB.m_data.m_noiseSize = 1.0f;
		m_noiseTextureComputeShaderCB.mapToGPU();

		m_deviceContext->CSSetConstantBuffers(0, 1, m_noiseTextureComputeShaderCB.getAddressOf());

		// Create an empty 3d texture to write to
		D3D11_TEXTURE3D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = size;
		textureDesc.Height = height;
		textureDesc.Depth = size;
		textureDesc.MipLevels = 0;
		textureDesc.Format = DXGI_FORMAT_R8_SNORM;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		

		HRESULT hr = m_device->CreateTexture3D(&textureDesc, nullptr, m_noiseTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create noise texture");

		// Create an unordered access view for the 3d texture
		D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
		ZeroMemory(&unorderedAccessViewDesc, sizeof(unorderedAccessViewDesc));

		unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		unorderedAccessViewDesc.Format = DXGI_FORMAT_R8_SNORM;
		unorderedAccessViewDesc.Texture3D.MipSlice = 0;
		unorderedAccessViewDesc.Texture3D.FirstWSlice = 0;
		unorderedAccessViewDesc.Texture3D.WSize = size;

		hr = m_device->CreateUnorderedAccessView(m_noiseTexture.Get(), &unorderedAccessViewDesc, m_noiseTextureUnorderedAccessView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create noise texture UAV");

		// Set the new UAV
		m_deviceContext->CSSetUnorderedAccessViews(0, 1, m_noiseTextureUnorderedAccessView.GetAddressOf(), nullptr);

		// Do some epik GPU computations to the UAV
		m_deviceContext->Dispatch(size / 8, height / 8, size / 8);

		//Unbind the UAV
		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };

		m_deviceContext->CSSetShader(NULL, NULL, 0);
		m_deviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);
		m_deviceContext->CSSetShaderResources(0, 1, ppSRVNULL);

		// Create a shader resource view from the resultant data
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R8_SNORM;//DXGI_FORMAT_R16G16B16A16_FLOAT
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		shaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture3D.MipLevels = 1;

		hr = m_device->CreateShaderResourceView(m_noiseTexture.Get(), &shaderResourceViewDesc, m_noiseTextureShaderResourceView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create noise texture SRV");
	}

	bool GraphicsHandler::loadScene(const char* sceneName)
	{
		if (m_sceneLoaded)
		{
			unloadScene();
		}

		m_sceneLoadedName = sceneName;

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
					XMFLOAT3 nodePos = readFloat3(sceneFile);

					XMFLOAT3 nodeLookAt = readFloat3(sceneFile);

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
					if (!renderableGameObject->initialize(label, fileName, m_device.Get(), m_deviceContext.Get()))
					{
						std::cout << "Failed to init renderable game object with label " << label << "\n";
						return false;
					}

					gameObject = dynamic_cast<entity::GameObject*>(renderableGameObject);
					break;
				}
				case entity::GameObjectType::eLight:
				{
					entity::LightGameObject* directionalLight = new entity::LightGameObject();

					if (!directionalLight->initialize(label, m_device.Get(), m_deviceContext.Get()))
					{
						std::cout << "Failed to init directional light with label " << label << "\n";
						return false;
					}

					directionalLight->setColour(readFloat3(sceneFile));
					gameObject = dynamic_cast<entity::GameObject*>(directionalLight);
					break;
				}
				case entity::GameObjectType::ePointLight:
				{
					entity::PointLightGameObject* pointLight = new entity::PointLightGameObject();

					if (!pointLight->initialize(label, m_device.Get(), m_deviceContext.Get()))
					{
						std::cout << "Failed to init point light with label " << label << "\n";
						return false;
					}

					pointLight->setColour(readFloat3(sceneFile));
					gameObject = dynamic_cast<entity::GameObject*>(pointLight);
					break;
				}
				case entity::GameObjectType::eSpotLight:
				{
					entity::SpotLightGameObject* spotLight = new entity::SpotLightGameObject();

					if (!spotLight->initialize(label, m_device.Get(), m_deviceContext.Get()))
					{
						std::cout << "Failed to init spot light with label " << label << "\n";
						return false;
					}

					spotLight->setColour(readFloat3(sceneFile));

					float innerCutoff, outerCutoff;
					sceneFile >> innerCutoff >> outerCutoff;
					spotLight->setInnerCutoff(innerCutoff);
					spotLight->setOuterCutoff(outerCutoff);

					gameObject = dynamic_cast<entity::GameObject*>(spotLight);
					break;
				}
				case entity::GameObjectType::ePhysics:
				{
					entity::physics::PhysicsGameObject* physicsGameObject = new entity::physics::PhysicsGameObject();

					std::string fileName;
					sceneFile >> fileName;
					utils::string_helpers::replaceChars(fileName, '|', ' ');
					fileName = "res/models/" + fileName;

					if (!physicsGameObject->initialize(label, fileName, m_device.Get(), m_deviceContext.Get()))
					{
						std::cout << "Failed to init physics object with label " << label << "\n";
						return false;
					}

					gameObject = dynamic_cast<entity::GameObject*>(physicsGameObject);
					break;
				}
				default:
				{
					gameObject = new entity::GameObject();
					break;
				}
				}

				gameObject->getTransform().setPosition(readFloat3(sceneFile));

				int rotationType;
				sceneFile >> rotationType;

				switch (rotationType)
				{
				case 1:
				{
					XMFLOAT3 front = readFloat3(sceneFile);
					XMFLOAT3 right = readFloat3(sceneFile);
					XMFLOAT3 up = readFloat3(sceneFile);

					// Outdated method

					/*gameObject->GetTransform()->SetFrontVector(XMVectorSet(front.x, front.y, front.z, 0.0f));
					gameObject->GetTransform()->SetRightVector(XMVectorSet(right.x, right.y, right.z, 0.0f));
					gameObject->GetTransform()->SetUpVector(XMVectorSet(up.x, up.y, up.z, 0.0f));*/

					break;
				}
					
				case 2:
					gameObject->getTransform().lookAtPosition(readFloat3(sceneFile));
					break;
				case 3:
				{
					XMFLOAT4 orientation = readFloat4(sceneFile);
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
					XMFLOAT3 cameraRelativePosition = readFloat3(sceneFile);
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

				m_gameObjectMap.insert({ gameObject->getLabel(), gameObject });

				switch (objectType)
				{
				case entity::GameObjectType::ePhysics:
				{
					m_physicsGameObjects.push_back(dynamic_cast<entity::physics::PhysicsGameObject*>(gameObject));
				}
				case entity::GameObjectType::eRenderable:
				{
					m_renderableGameObjects.push_back(dynamic_cast<entity::RenderableGameObject*>(gameObject));
					break;
				}
				case entity::GameObjectType::ePointLight:
				{
					m_pointLights.push_back(dynamic_cast<entity::PointLightGameObject*>(gameObject));
					break;
				}
				case entity::GameObjectType::eSpotLight:
				{
					m_spotLights.push_back(dynamic_cast<entity::SpotLightGameObject*>(gameObject));
					break;
				}
				}
			}

			m_sceneLoaded = true;

			return true;
		}

		return false;
	}

	bool GraphicsHandler::saveScene()
	{
		std::string sceneFilePath = "res/scenes/";
		sceneFilePath += m_sceneLoadedName;

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
						writeFloat3(trackNodes->at(i).m_position, sceneFile);
						writeFloat3(trackNodes->at(i).m_lookPoint, sceneFile);

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
						writeFloat3(directionalLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObjectType::ePointLight:
					{
						entity::PointLightGameObject* pointLight = dynamic_cast<entity::PointLightGameObject*>(objectMapIterator->second);
						writeFloat3(pointLight->getColour(), sceneFile);
						break;
					}
					case entity::GameObjectType::eSpotLight:
					{
						entity::SpotLightGameObject* spotLight = dynamic_cast<entity::SpotLightGameObject*>(objectMapIterator->second);
						writeFloat3(spotLight->getColour(), sceneFile);
						sceneFile << spotLight->getInnerCutoff() << " " << spotLight->getOuterCutoff() << " ";
						break;
					}
					}

					sceneFile << '\n';

					writeFloat3(objectMapIterator->second->getTransform().getPositionFloat3(), sceneFile);
					sceneFile << '\n';

					sceneFile << 3 << ' ';
					writeFloat4(objectMapIterator->second->getTransform().getOrientation(), sceneFile);
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
						writeFloat3(objectMapIterator->second->getRelativePositions()->at(i), sceneFile);
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

	bool GraphicsHandler::saveSceneTGP()
	{
		std::string sceneFilePath = "res/scenes/";
		sceneFilePath = sceneFilePath + "tgp_" + m_sceneLoadedName;

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

	void GraphicsHandler::unloadScene()
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

		if (m_selectingGameObject)
		{
			m_selectingGameObject = false;
			m_selectedObject = nullptr;
			m_lastAxisGrabOffset = FLT_MAX;
			m_lastGrabPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		}
	
		m_sceneLoaded = false;
	}

	void GraphicsHandler::removeGameObject(const std::string& gameObjectLabel)
	{
		//If the object exists in the map
		std::unordered_map<std::string, entity::GameObject*>::iterator iterator = m_gameObjectMap.find(gameObjectLabel);
		if (iterator != m_gameObjectMap.end())
		{
			//Remove controller
			if (iterator->second->getController() != nullptr)
			{
				std::vector<entity::GameObjectController>& controllers = m_controllerManager->getControllers();
				size_t numControllers = controllers.size();
				for (size_t i = 0; i < numControllers; ++i)
				{
					if (iterator->second->getController() == &controllers.at(i))
					{
						controllers.erase(controllers.begin() + i);
						break;
					}
				}
			}
		
			// Remove from renderables
			for (int i = 0; i < m_renderableGameObjects.size(); ++i)
			{
				if (iterator->second == m_renderableGameObjects.at(i))
				{
					m_renderableGameObjects.erase(m_renderableGameObjects.begin() + i);
				}
			}

			// Remove from physics
			for (int i = 0; i < m_physicsGameObjects.size(); ++i)
			{
				if (iterator->second == m_physicsGameObjects.at(i))
				{
					m_physicsGameObjects.erase(m_physicsGameObjects.begin() + i);
				}
			}

			// Remove from spot lights
			for (int i = 0; i < m_spotLights.size(); ++i)
			{
				if (iterator->second == m_spotLights.at(i))
				{
					m_spotLights.erase(m_spotLights.begin() + i);
				}
			}

			// Remove from point lights
			for (int i = 0; i < m_pointLights.size(); ++i)
			{
				if (iterator->second == m_pointLights.at(i))
				{
					m_pointLights.erase(m_pointLights.begin() + i);
				}
			}

			delete iterator->second;

			m_gameObjectMap.erase(iterator->first);
		}
	}

	void GraphicsHandler::updateImGui()
	{
		//START NEW FRAME
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//New Object Menu
		if (m_newObjectMenuOpen)
		{
			if (ImGui::Button("Create Object"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_newObjectMenuOpen = false;
			}
		}

		//SELECTED OBJECT MENU
		if (m_selectingGameObject)
		{
			ImGui::Begin("Game Object Settings"); //TITLE
			ImGui::Text(("Label: " + m_selectedObject->getLabel()).c_str()); //OBJECT LABEL
			ImGui::InputText("Object Label", &m_selectedObject->getLabelPtr()->at(0), 20);
			XMFLOAT3 pos = m_selectedObject->getTransform().getPositionFloat3();
			ImGui::Text(("X: " + std::to_string(pos.x) + "Y: " + std::to_string(pos.y) + "Z: " + std::to_string(pos.z)).c_str()); //POSITION
			if (m_selectedObject->getObjectTrack() != nullptr)
			{ //FOLLOW TRACK CHECKBOX
				bool followingTrack = m_selectedObject->getFollowingObjectTrack();
				ImGui::Checkbox("Follow Track", &followingTrack);
				m_selectedObject->setFollowingObjectTrack(followingTrack);
			}
			bool floatingObject = m_selectedObject->getFloating();
			ImGui::Checkbox("Float Object", &floatingObject);
			m_selectedObject->setFloating(floatingObject);

			if (m_selectedObject->getIsFollowingObject())
			{
				if (ImGui::Button("Stop Following Object"))
				{
					m_selectedObject->setIsFollowingObject(false);
					m_selectedObject->setObjectToFollow(nullptr);
				}
			}
			else {
				if (ImGui::TreeNode("Followable Objects"))
				{
					std::unordered_map<std::string, entity::GameObject*>::iterator mapIterator = m_gameObjectMap.begin();
					while (mapIterator != m_gameObjectMap.end())
					{
						entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second);

						std::string label = "Follow " + gameObject->getLabel();

						if (ImGui::Button(label.c_str()))
						{
							m_selectedObject->setObjectToFollow(gameObject);
							m_selectedObject->setIsFollowingObject(true);
						}
						mapIterator++;
					}
					ImGui::TreePop();
				}
			}
		
			if (ImGui::Button("Edit Translation"))
			{
				m_axisEditState = AxisEditState::eEditTranslate;
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit Rotation"))
			{
				m_axisEditState = AxisEditState::eEditRotate;
			}
			if (ImGui::Button("Reset Rotation"))
			{
				m_selectedObject->getTransform().setOrientationQuaternion(XMQuaternionIdentity());
			}

			entity::GameObjectType type = m_selectedObject->getType();
			if (type == entity::GameObjectType::eRenderable)
			{//IF IT IS A RENDERABLE, ALLOW SCALING
				XMFLOAT3 scale = m_selectedObject->getScale();
				ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.1f, 10.0f);
				m_selectedObject->setScale(scale);
			}
			else if (type == entity::GameObjectType::ePointLight || type == entity::GameObjectType::eSpotLight)
			{ // IF IT IS A LIGHT, ALLOW COLOUR CHANGE AND MOVE TO CAMERA
				entity::LightGameObject* lightObj = reinterpret_cast<entity::LightGameObject*>(m_selectedObject);
				ImGui::ColorEdit3("Colour", &(lightObj->m_colour.x));
				if (ImGui::Button("Move to Camera"))
				{
					XMVECTOR lightPosition = m_camera.getTransform().getPositionVector();
					lightPosition += m_camera.getTransform().getFrontVector();
					lightObj->getTransform().setPosition(lightPosition);
					lightObj->getTransform().copyOrientationFrom(m_camera.getTransform());
					lightObj->setFollowingObjectTrack(false);
				}

				entity::PointLightGameObject* pointLightObj = reinterpret_cast<entity::PointLightGameObject*>(lightObj);
				ImGui::SliderFloat("Att. Const.", pointLightObj->getAttenuationConstantPtr(), 0.05f, 1.0f);
				ImGui::SliderFloat("Att. Lin.", pointLightObj->getAttenuationLinearPtr(), 0.0f, 0.2f);
				ImGui::SliderFloat("Att. Quad.", pointLightObj->getAttenuationQuadraticPtr(), 0.0f, 0.05f);

				if (type == entity::GameObjectType::eSpotLight)
				{
					entity::SpotLightGameObject* spotLightObj = reinterpret_cast<entity::SpotLightGameObject*>(lightObj);
					ImGui::SliderFloat("Inn. Cut.", spotLightObj->getInnerCutoffPtr(), 0.0f, 90.0f);
					ImGui::SliderFloat("Out. Cut.", spotLightObj->getOuterCutoffPtr(), 0.0f, 90.0f);
				}
			}

			size_t numRelativeCameras = m_selectedObject->getRelativePositions()->size();
			if (numRelativeCameras != 0)
			{
				ImGui::Text("Relative Cameras");
			}
			for (size_t i = 0; i < numRelativeCameras; ++i)
			{
				if (ImGui::Button(("Camera " + std::to_string(i + 1)).c_str()))
				{
					m_camera.setRelativeObject(m_selectedObject, m_selectedObject->getRelativePositions()->at(i));
					m_camera.setFollowingObjectTrack(false);
				}
				if (i < numRelativeCameras - 1)
				{
					ImGui::SameLine();
				}
			}

			if (m_selectedObject->getController() != nullptr)
			{
				ImGui::Checkbox("Control Object", m_selectedObject->getController()->isActivePtr());
			}

			if (type == entity::GameObjectType::ePhysics)
			{
				entity::physics::PhysicsGameObject* object = dynamic_cast<entity::physics::PhysicsGameObject*>(m_selectedObject);
				ImGui::Checkbox("Static", object->getRigidBody()->isStaticPtr());
				if (ImGui::Button("Apply Upwards Thrust"))
				{
					object->getRigidBody()->addThrust(XMVectorSet(0.0f, 850.0f, 0.0f, 0.0f), 0.5f);
				}
				if (ImGui::Button("Apply Forward Thrust"))
				{
					object->getRigidBody()->addThrust(XMVector3Normalize(object->getTransform().getPositionVector() - m_camera.getTransform().getPositionVector()) * 800.0f, 0.5f);
				}
				if (ImGui::Button("Apply Torque"))
				{
					XMVECTOR worldPos = (object->getTransform().getPositionVector() + m_camera.getTransform().getPositionVector()) * 0.5f;
					object->getRigidBody()->addTorque(worldPos - object->getTransform().getPositionVector(), XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f));
				}
				if (ImGui::Button("Apply Split Force"))
				{
					XMVECTOR worldPos = m_camera.getTransform().getPositionVector();
					object->getRigidBody()->addForceSplit(worldPos, m_camera.getTransform().getFrontVector() * 10000.0f);
				}
			}

			if (ImGui::Button("Delete Object"))
			{
				removeGameObject(m_selectedObject->getLabel());
				m_selectedObject = nullptr;
				m_selectingGameObject = false;
			}

			if (ImGui::Button("Close"))
			{
				m_selectedObject = nullptr;
				m_selectingGameObject = false;
			}
		
			ImGui::End();
		}

		ImGui::Begin("Object Selection");
	
		/*if (ImGui::BeginMenu("Objects"))
		{*/
		std::unordered_map<std::string, entity::GameObject*>::iterator mapIterator = m_gameObjectMap.begin();
		while (mapIterator != m_gameObjectMap.end())
		{
			entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second);

			std::string label = gameObject->getLabel();

			if (m_selectedObject == gameObject)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ("> " + label).c_str());
			}
			else if (ImGui::MenuItem(label.c_str()))
			{
				m_selectedObject = gameObject;
				m_selectingGameObject = true;
			}
			mapIterator++;
		}
		//ImGui::EndMenu();
		//}
	

		if (m_selectingGameObject)
		{
			if (ImGui::Button("Previous Object"))
			{
				std::unordered_map<std::string, entity::GameObject*>::iterator mapIterator = m_gameObjectMap.begin();
				while (++mapIterator != m_gameObjectMap.end())
				{
					if (mapIterator->second == m_selectedObject)
					{
						if (--mapIterator == m_gameObjectMap.begin())
						{
							m_selectedObject = dynamic_cast<entity::RenderableGameObject*>((--m_gameObjectMap.end())->second);
						}
						else {
							m_selectedObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second);
						}
						break;
					}
				}
			}
			if (ImGui::Button("Next Object"))
			{
				std::unordered_map<std::string, entity::GameObject*>::iterator mapIterator = m_gameObjectMap.begin();
				while (++mapIterator != m_gameObjectMap.end())
				{
					if (mapIterator->second == m_selectedObject)
					{
						if (++mapIterator == m_gameObjectMap.end())
						{
							m_selectedObject = dynamic_cast<entity::RenderableGameObject*>((++m_gameObjectMap.begin())->second);
						}
						else {
							m_selectedObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second);
						}
						break;
					}
				}
			}
		}

		ImGui::End();

		ImGui::Begin("Scene Settings");
	
		if (ImGui::Button("Save Scene"))
		{
			saveScene();
		}
		ImGui::SameLine();
		if (ImGui::Button("Export Scene"))
		{
			saveSceneTGP();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load city.txt"))
		{
			loadScene("city.txt");
		}
		ImGui::SameLine();
		if (ImGui::Button("Unload Scene"))
		{
			unloadScene();
		}
		static char label[20] = "";
		static char path[50] = "city/wall/frames/left/r.obj";
		ImGui::InputText("New Object Label", &label[0], 20);
		ImGui::InputText("New Object Path", &path[0], 50);

		if (ImGui::Button("Spawn Object"))
		{
			std::string labelChosen = label;
			if (labelChosen != "" && m_gameObjectMap.find(labelChosen) == m_gameObjectMap.end())
			{
				entity::RenderableGameObject* newObject = new entity::RenderableGameObject();
				std::string pathstr = path;
				newObject->initialize(label, "res/models/environment/meshes/" + pathstr, m_device.Get(), m_deviceContext.Get());

				newObject->getTransform().setPosition(m_camera.getTransform().getPositionVector() + m_camera.getTransform().getFrontVector() * 5.0f);

				m_renderableGameObjects.push_back(newObject);
				m_gameObjectMap.insert(std::make_pair(newObject->getLabel(), newObject));

				m_selectedObject = newObject;
				m_selectingGameObject = true;
			}
		}

		//DIRECTIONAL LIGHT COLOUR
		ImGui::ColorEdit3("Dir Light Colour", &(m_directionalLight.m_colour.x));
		m_cloudsPixelShaderCB.m_data.m_lightColour = m_directionalLight.m_colour;

		//NORMAL MAPPING CHECKBOX
		bool useNormalMapping = static_cast<bool>(m_pixelShaderCB.m_data.m_useNormalMapping);
		ImGui::Checkbox("Normal Mapping", &useNormalMapping);
		m_pixelShaderCB.m_data.m_useNormalMapping = useNormalMapping;

		ImGui::SameLine();

		//POM CHECKBOX
		bool useParallaxOcclusionMapping = static_cast<bool>(m_pixelShaderCB.m_data.m_useParallaxOcclusionMapping);
		ImGui::Checkbox("PO Mapping", &useParallaxOcclusionMapping);
		m_pixelShaderCB.m_data.m_useParallaxOcclusionMapping = useParallaxOcclusionMapping;

		ImGui::SameLine();

		//WIREFRAME CHECKBOX
		ImGui::Checkbox("Wireframe", &m_useWireframe);

		//POM HEIGHT CHECKBOX
		static float parallaxOcclusionMappingHeight = 0.025f;
		ImGui::DragFloat("PO Mapping Height", &parallaxOcclusionMappingHeight, 0.001f, 0.0f, 0.2f);
		m_pixelShaderCB.m_data.m_parallaxOcclusionMappingHeight = parallaxOcclusionMappingHeight;

		//VSYNC CHECKBOX
		ImGui::Checkbox("Use VSync", &m_useVSync);

		ImGui::NewLine();

		if (ImGui::Button("Toggle Time Progression")) m_dayNightCycle = !m_dayNightCycle;
		ImGui::SliderFloat("Day Progress", &m_dayProgress, 0.0f, 1.0f);
		
		if (ImGui::CollapsingHeader("Ocean Options"))
		{
			ImGui::SliderInt("Wave Count", &m_waterVertexShaderCB.m_data.m_waveCount, 0, 20);
			ImGui::SliderFloat("Wave Scale", &m_waterVertexShaderCB.m_data.m_waveScale, 0.0f, 10.0f);
			ImGui::SliderFloat("Wave Period", &m_waterVertexShaderCB.m_data.m_wavePeriod, 0.0f, 50.0f);
			ImGui::SliderFloat("Wave Speed", &m_waterVertexShaderCB.m_data.m_waveSpeed, 0.0f, 50.0f);
			ImGui::SliderFloat("Wave Seed", &m_waterVertexShaderCB.m_data.m_waveSeed, 100.0f, 1000.0f);
			ImGui::SliderFloat("Wave Scale Multiplier", &m_waterVertexShaderCB.m_data.m_waveScaleMultiplier, 0.0f, 1.0f);

			m_waterPixelShaderCB.m_data.m_waveCount = m_waterVertexShaderCB.m_data.m_waveCount;
			m_waterPixelShaderCB.m_data.m_waveScale = m_waterVertexShaderCB.m_data.m_waveScale;
			m_waterPixelShaderCB.m_data.m_wavePeriod = m_waterVertexShaderCB.m_data.m_wavePeriod;
			m_waterPixelShaderCB.m_data.m_waveSpeed = m_waterVertexShaderCB.m_data.m_waveSpeed;
			m_waterPixelShaderCB.m_data.m_waveSeed = m_waterVertexShaderCB.m_data.m_waveSeed;
			m_waterPixelShaderCB.m_data.m_waveScaleMultiplier = m_waterVertexShaderCB.m_data.m_waveScaleMultiplier;

			ImGui::SliderFloat("Foam Start", &m_waterPixelShaderCB.m_data.m_foamStart, 0.0f, 20.0f);
			ImGui::SliderFloat("Colour Change Start", &m_waterPixelShaderCB.m_data.m_colourChangeStart, 0.0f, 1.5f);

		}

		if (ImGui::CollapsingHeader("Atmosphere Options"))
		{
			ImGui::SliderFloat("Sun Size", &m_atmosphericPixelShaderCB.m_data.m_sunSize, 10.0f, 200.0f);
			ImGui::SliderFloat("Density", &m_atmosphericPixelShaderCB.m_data.m_density, 0.0f, 3.0f);
			ImGui::SliderFloat("Multi Scatter Phase", &m_atmosphericPixelShaderCB.m_data.m_multiScatterPhase, 0.0f, 3.0f);
			ImGui::SliderFloat("Anisotropic Intensity", &m_atmosphericPixelShaderCB.m_data.m_anisotropicIntensity, -1.0f, 5.0f);
			ImGui::SliderFloat("Zenith Offset", &m_atmosphericPixelShaderCB.m_data.m_zenithOffset, -1.0f, 1.0f);
			ImGui::SliderFloat("Night Density", &m_atmosphericPixelShaderCB.m_data.m_nightDensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Night Zenith Y Clamp", &m_atmosphericPixelShaderCB.m_data.m_nightZenithYClamp, 0.0f, 0.1f);
		}

		if (ImGui::CollapsingHeader("Clouds Options"))
		{
			ImGui::SliderFloat("Absorbtion Through Clouds", &m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionThroughClouds, 0.0f, 2.0f);
			ImGui::SliderFloat("Absorbtion Towards Sun", &m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionTowardsSun, 0.0f, 2.0f);
			ImGui::SliderFloat("Phase Factor", &m_cloudsPixelShaderCB.m_data.m_phaseFactor, 0.0f, 2.0f);
			ImGui::SliderFloat("Darkness Threshold", &m_cloudsPixelShaderCB.m_data.m_darknessThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Cloud Coverage", &m_cloudsPixelShaderCB.m_data.m_cloudCoverage, 0.0f, 1.0f);
			ImGui::SliderFloat("Cloud Speed", &m_cloudsPixelShaderCB.m_data.m_cloudSpeed, 0.0f, 0.25f);
			ImGui::SliderFloat("Cloud Height", &m_cloudsPixelShaderCB.m_data.m_cloudHeight, 100.0f, 2000.0f);
			ImGui::SliderInt("Num Steps", &m_cloudsPixelShaderCB.m_data.m_numSteps, 1, 100);
			ImGui::SliderFloat("Step Size", &m_cloudsPixelShaderCB.m_data.m_stepSize, 5.0f, 100.0f);
		}

		ImGui::End();


		ImGui::Begin("Particle System Settings");

		ImGui::SliderFloat3("Position", &m_particleSystem->getEmitters()->at(0)->getPosition()->m128_f32[0], -10.0f, 10.0f);

		XMVECTOR* ParticleEmitterDirection = m_particleSystem->getEmitters()->at(0)->getDirection();
		ImGui::SliderFloat3("Direction", &ParticleEmitterDirection->m128_f32[0], -1.0f, 1.0f);
		*ParticleEmitterDirection = XMVector3Normalize(*ParticleEmitterDirection);

		ImGui::SliderFloat("Direction Randomness", m_particleSystem->getEmitters()->at(0)->getDirectionRandomnessPtr(), 0.0f, 1.0f);

		ImGui::SliderFloat("Power", m_particleSystem->getEmitters()->at(0)->getPowerPtr(), 0.0f, 10.0f);
		ImGui::SliderFloat("Power Randomness", m_particleSystem->getEmitters()->at(0)->getPowerRandomModifierPtr(), 0.0f, 1.0f);

		ImGui::SliderFloat("Max Age", m_particleSystem->getEmitters()->at(0)->getMaxAgePtr(), 0.0f, 5.0f);
		ImGui::SliderFloat("Max Age Randomness", m_particleSystem->getEmitters()->at(0)->getMaxAgeRandomModifierPtr(), 0.0f, 1.0f);

		ImGui::SliderFloat("Spawn Delay", m_particleSystem->getEmitters()->at(0)->getSpawnDelayPtr(), 0.0f, 0.5f);
		ImGui::SliderFloat("Spawn Delay Rand", m_particleSystem->getEmitters()->at(0)->getSpawnDelayRandomModifierPtr(), 0.0f, 1.0f);
	
		ImGui::End();


		ImGui::Begin("Camera Settings");
		//CAMERA TRACK CHECKBOX
		bool followTrack = m_camera.getFollowingObjectTrack();
		ImGui::Checkbox("Camera Follow Track", &followTrack);
		m_camera.setFollowingObjectTrack(followTrack);

		//RELATIVE CAMERA CHECKBOX
		ImGui::SameLine();
		if (ImGui::Button("Exit Relative Camera"))
		{
			*m_camera.getUsingRelativeCameraPtr() = false;
		}

		ImGui::Checkbox("Control Camera", m_controllerManager->getControllers().at(0).isActivePtr());

		//CAMERA TRACK DELTA
		//float cameraTrackDelta = camera.GetObjectTrackDelta();
		//ImGui::DragFloat("Camera Track Delta", &cameraTrackDelta, 0.005f, -0.5f, 10.0f);
		//camera.SetObjectTrackDelta(cameraTrackDelta);

		if (ImGui::Button("Static View One"))
		{
			*m_camera.getUsingRelativeCameraPtr() = false;
			m_camera.setFollowingObjectTrack(false);
			m_camera.getTransform().setPosition(XMFLOAT3(0.0f, 37.5f, 0.0f));
			m_camera.getTransform().lookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.5f));
			m_camera.setZoom(80.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Static View Two"))
		{
			*m_camera.getUsingRelativeCameraPtr() = false;
			m_camera.setFollowingObjectTrack(false);
			m_camera.getTransform().setPosition(XMFLOAT3(10.0f, 10.0f, 10.0f));
			m_camera.getTransform().lookAtPosition(XMFLOAT3(3.0f, 2.5f, 3.0f));
			m_camera.setZoom(70.0f);
		}

		ImGui::End();
	}

	entity::CameraGameObject& GraphicsHandler::getCamera()
	{
		return m_camera;
	}

	entity::GameObject* GraphicsHandler::getGameObject(const std::string& label)
	{
		if (m_gameObjectMap.find(label) != m_gameObjectMap.end())
		{
			return m_gameObjectMap.at(label);
		}
	}

	void GraphicsHandler::update(float deltaTime)
	{
		//UPDATE MOUSE NDC
		computeMouseNDC();

		//UPDATE MOUSE TO WORLD VECTOR
		m_camera.computeMouseToWorldVectorDirection(m_mouseNDCX, m_mouseNDCY);

		//UPDATE SELECTED OBJECT (TRANSLATION / ROTATION)
		if (m_selectingGameObject)
		{
			updateSelectedObject();
		}

		//UPDATE GAME OBJECTS
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
	
		//particleSystem->Update(deltaTime);

		//UPDATE IMGUI
		updateImGui();
	}

	void GraphicsHandler::checkObjectCollisions(float deltaTime)
	{
		for (int i = 0; i < m_physicsGameObjects.size(); ++i)
		{
			if (!m_physicsGameObjects[i]->getRigidBody()->isStatic())
			{
				for (int j = i + 1; j < m_physicsGameObjects.size(); ++j)
				{
					if (m_physicsGameObjects[i]->getWorldSpaceBoundingBox().Intersects(m_physicsGameObjects[j]->getWorldSpaceBoundingBox()))
					{
						float velocityOne = std::max(XMVectorGetX(XMVector3Length(m_physicsGameObjects[i]->getRigidBody()->getVelocityVector())), 1.0f);
						float velocityTwo = std::max(XMVectorGetX(XMVector3Length(m_physicsGameObjects[j]->getRigidBody()->getVelocityVector())), 1.0f);

						float forceMagnitude = (m_physicsGameObjects[i]->getMass() * velocityOne + m_physicsGameObjects[j]->getMass() * velocityTwo) / deltaTime;
						XMVECTOR force = XMVector3Normalize(m_physicsGameObjects[j]->getTransform().getPositionVector() - m_physicsGameObjects[i]->getTransform().getPositionVector()) * forceMagnitude * 0.15f; //remove force (coefficient of restitution)
						m_physicsGameObjects[i]->getRigidBody()->addForce(-force);
						m_physicsGameObjects[j]->getRigidBody()->addForce(force);

						/*XMVECTOR relativeOne = 0.5f * (physicsGameObjects[j]->GetTransform()->GetPositionVector() - physicsGameObjects[i]->GetTransform()->GetPositionVector());
						XMVECTOR forceOne = physicsGameObjects[i]->GetRigidBody()->GetForceAtRelativePosition(relativeOne);

						XMVECTOR relativeTwo = 0.5f * (physicsGameObjects[i]->GetTransform()->GetPositionVector() - physicsGameObjects[j]->GetTransform()->GetPositionVector());
						XMVECTOR forceTwo = physicsGameObjects[j]->GetRigidBody()->GetForceAtRelativePosition(relativeTwo);

						physicsGameObjects[i]->GetRigidBody()->AddForce(-force);
						physicsGameObjects[j]->GetRigidBody()->AddForce(force);

						physicsGameObjects[i]->GetRigidBody()->AddTorque(relativeOne, (forceTwo * 0.5f - forceOne * 0.5f) * 0.005f);
						physicsGameObjects[j]->GetRigidBody()->AddTorque(relativeTwo, (forceOne * 0.5f - forceTwo * 0.5f) * 0.005f);*/
					}
				}

				std::vector<XMFLOAT3>* vertices = m_physicsGameObjects[i]->getModel()->getVertices();
				XMFLOAT3 objectPosition = m_physicsGameObjects[i]->getTransform().getPositionFloat3();
				for (int j = 0; j < vertices->size(); ++j)
				{
					XMFLOAT3 vertexPosition = vertices->at(j);
					XMVECTOR rotatedPosition = XMVector3Transform(XMLoadFloat3(&vertexPosition), m_physicsGameObjects[i]->getTransform().getRotationMatrix());
					XMStoreFloat3(&vertexPosition, rotatedPosition);

					float diff = objectPosition.y + vertexPosition.y + 2.0f;
					if (diff < 0.0f)
					{
						m_physicsGameObjects[i]->getTransform().setPosition(m_physicsGameObjects[i]->getTransform().getPositionVector() + XMVectorSet(0.0f, -diff, 0.0f, 0.0f));
						objectPosition = m_physicsGameObjects[i]->getTransform().getPositionFloat3();

						float force = (XMVectorGetX(XMVector3Length(m_physicsGameObjects[i]->getRigidBody()->getVelocityVector())) * m_physicsGameObjects[i]->getRigidBody()->getMass()) / deltaTime;
						//float force = XMVectorGetY(physicsGameObjects[i]->GetRigidBody()->GetForceAtRelativePosition(rotatedPosition));
						m_physicsGameObjects[i]->getRigidBody()->addForce(XMVectorSet(0.0f, force * 0.8f, 0.0f, 0.0f));
						m_physicsGameObjects[i]->getRigidBody()->addTorque(rotatedPosition, XMVectorSet(0.0f, force * 0.005f, 0.0f, 0.0f));
						//float dot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(force), XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f)));
						//if (dot > 0.0f)
						//{
						//	physicsGameObjects[i]->GetRigidBody()->AddForce(force * dot);//physicsGameObjects[i]->GetTransform()->GetPositionVector() + rotatedPosition, 
						//	//physicsGameObjects[i]->GetRigidBody()->AddTorque(rotatedPosition, -force * 0.005f);
						//}
					}
				}

				float upthrustMagnitude = (m_physicsGameObjects[i]->getRigidBody()->getMass() * 18.0f) / static_cast<float>(vertices->size());
				for (int j = 0; j < vertices->size(); ++j)
				{
					XMFLOAT3 vertexPosition = vertices->at(j);
					XMVECTOR rotatedPosition = XMVector3Transform(XMLoadFloat3(&vertexPosition), m_physicsGameObjects[i]->getTransform().getRotationMatrix());
					XMStoreFloat3(&vertexPosition, rotatedPosition);

					float diff = objectPosition.y + vertexPosition.y - getWaterHeightAt(objectPosition.x + vertexPosition.x, objectPosition.z + vertexPosition.z, true);
					if (diff < 0.0f)
					{
						//Buoyancy forces
						m_physicsGameObjects[i]->getRigidBody()->addForce(XMVectorSet(0.0f, upthrustMagnitude, 0.0f, 0.0f));
						m_physicsGameObjects[i]->getRigidBody()->addTorque(rotatedPosition, XMVectorSet(0.0f, upthrustMagnitude * 0.001f, 0.0f, 0.0f));

						//Water drag forces
						XMVECTOR waterDragForce = m_physicsGameObjects[i]->getRigidBody()->getMass() * -m_physicsGameObjects[i]->getRigidBody()->getVelocityVector() * 0.1f;
						m_physicsGameObjects[i]->getRigidBody()->addForce(waterDragForce);
					}
				}
			}
		}
	}

	void GraphicsHandler::adjustMouseX(int xPos)
	{
		m_mousePosX += xPos;
	}

	void GraphicsHandler::adjustMouseY(int yPos)
	{
		m_mousePosY += yPos;
	}

	void GraphicsHandler::setMouseX(int xPos)
	{
		m_mousePosX = xPos;
	}

	void GraphicsHandler::setMouseY(int yPos)
	{
		m_mousePosY = yPos;
	}

	void GraphicsHandler::computeMouseNDC()
	{
		m_mouseNDCX = (2.0f * static_cast<float>(m_mousePosX)) / (static_cast<float>(m_windowWidth)) - 1.0f;
		m_mouseNDCY = 1.0f - (2.0f * static_cast<float>(m_mousePosY)) / static_cast<float>(m_windowHeight);
	}

	bool GraphicsHandler::initializeDirectX(HWND hwnd)
	{
		try
		{
			//GET GRAPHICS CARD ADAPTERS
			std::vector<utils::AdapterData> adapters = utils::AdapterReader::getAdapters();

			if (adapters.size() < 1)
			{
				utils::ErrorLogger::log("No DXGI Adapters Found.");
				return false;
			}

			//CREATE DEVICE AND SWAPCHAIN
			DXGI_SWAP_CHAIN_DESC swapChainDescription;
			ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

			swapChainDescription.BufferDesc.Width = m_windowWidth;
			swapChainDescription.BufferDesc.Height = m_windowHeight;
			swapChainDescription.BufferDesc.RefreshRate.Numerator = 120; //VSYNC FPS
			swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;

			swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swapChainDescription.SampleDesc.Count = 1; //NO ANTI ALIASING
			swapChainDescription.SampleDesc.Quality = 0; //LOWEST IMAGE QUALITY

			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

			swapChainDescription.BufferCount = 1;

			swapChainDescription.OutputWindow = hwnd;

			swapChainDescription.Windowed = TRUE;

			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			HRESULT hr = D3D11CreateDeviceAndSwapChain( adapters[0].m_pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
														NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription,
														m_swapChain.GetAddressOf(), m_device.GetAddressOf(),
														NULL, m_deviceContext.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

			//GET BACKBUFFER
			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

			hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

			COM_ERROR_IF_FAILED(hr, "Failed to get back buffer.");

			//CREATE RENDER TARGET VIEW
			hr = m_device->CreateRenderTargetView(backBuffer.Get(), NULL, m_renderTargetView.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

			//CREATE DEPTH STENCIL TEXTURE AND VIEW
			CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_windowWidth, m_windowHeight);
			depthStencilTextureDesc.MipLevels = 1;
			depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			hr = m_device->CreateTexture2D(&depthStencilTextureDesc, NULL, m_depthStencilBuffer.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

			hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), NULL, m_depthStencilView.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

			//SET THE RENDER TARGET
			m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

			//CREATE DEPTH STENCIL STATE
			CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

			hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

			//CREATE VIEWPORT
			CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
			m_deviceContext->RSSetViewports(1, &viewport);

			//CREATE DEFAULT RASTERIZER STATE
			D3D11_RASTERIZER_DESC regularRasterizerDesc;
			ZeroMemory(&regularRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

			regularRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			regularRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			hr = m_device->CreateRasterizerState(&regularRasterizerDesc, m_regularRasterizerState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create default rasterizer state.");

			//CREATE WIREFRAME RASTERIZER STATE
			D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
			ZeroMemory(&wireFrameRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

			wireFrameRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
			wireFrameRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

			hr = m_device->CreateRasterizerState(&wireFrameRasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create wireframe rasterizer state.");

			//CREATE DEFAULT BLEND STATE
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));

			D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
			ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

			renderTargetBlendDesc.BlendEnable = true;
			renderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			renderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
			renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = renderTargetBlendDesc;

			hr = m_device->CreateBlendState(&blendDesc, m_blendState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

			//CREATE SPRITE BATCH AND SPRITE FONT INSTANCES
			m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_deviceContext.Get());
			m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"res\\fonts\\consolas16.spritefont");//comicSansMS16.spritefont

			//CREATE SAMPLER STATE
			CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
		}
		catch (utils::COMException & exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}
		return true;
	}

	void GraphicsHandler::checkSelectingObject()
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

	void GraphicsHandler::drawAxisForObject(entity::GameObject* gameObject, const XMMATRIX& viewProjection)
	{
		XMFLOAT3 gameObjectPosition = gameObject->getTransform().getPositionFloat3();
		XMMATRIX translationMatrix = XMMatrixTranslation(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);

		float scale = m_selectedObject->getModel()->getHitRadius();

		//CLAMP SCALE
		if (scale < 0.75f) scale = 0.75f;

		float distance = XMVectorGetX(XMVector3Length(m_selectedObject->getTransform().getPositionVector() - m_camera.getTransform().getPositionVector()));
		scale *= distance * 0.5;

		if (m_axisEditState == AxisEditState::eEditTranslate)
		{
			m_axisTranslateModel.draw(XMMatrixScaling(scale, scale, scale) * translationMatrix, viewProjection, &m_vertexShaderCB);
		}
		else if (m_axisEditState == AxisEditState::eEditRotate)
		{ //Multiply by rotation matrix when rotating
			m_axisRotateModel.draw(XMMatrixScaling(scale * 0.5f, scale * 0.5f, scale * 0.5f) * m_selectedObject->getTransform().getRotationMatrix() * translationMatrix, viewProjection, &m_vertexShaderCB);
		}
	}

	AxisEditSubState GraphicsHandler::getAxisEditSubState()
	{
		return m_axisEditSubState;
	}

	void GraphicsHandler::stopAxisEdit()
	{
		m_axisEditSubState = AxisEditSubState::eEditNone;
		m_lastAxisGrabOffset = FLT_MAX;
	}

	XMVECTOR GraphicsHandler::rayPlaneIntersect(XMVECTOR rayPoint, XMVECTOR rayDirection, XMVECTOR planeNormal, XMVECTOR planePoint)
	{
		XMVECTOR diff = rayPoint - planePoint;
		return (diff + planePoint) + rayDirection * (-XMVector3Dot(diff, planeNormal) / XMVector3Dot(rayDirection, planeNormal));
	}

	float GraphicsHandler::getWaterHeightAt(float posX, float posZ, bool exact)
	{
		float value = 0.0f;
		value += sin(-posX * 0.4f + m_gameTime * 1.2f) * 0.15f + sin(posZ * 0.5f + m_gameTime * 1.3f) * 0.15f;
		value += sin(posX * 0.2f + m_gameTime * 0.6f) * 0.5f + sin(-posZ * 0.22f + m_gameTime * 0.4f) * 0.45f;
		if (exact)
		{
			value += sin(posX * 1.5f + m_gameTime * 0.0017f) * 0.05f + sin(posZ * 1.5f + m_gameTime * 0.0019f) * 0.05f;
		}
		return value * m_vertexShaderCB.m_data.m_padding;
	}

	void GraphicsHandler::floatObject(entity::GameObject* object)
	{
		XMFLOAT3 positionFloat = object->getTransform().getPositionFloat3();
		XMVECTOR position = XMVectorSet(0.0f, getWaterHeightAt(positionFloat.x, positionFloat.z), 0.0f, 0.0f);

		object->getTransform().setPosition(XMFLOAT3(positionFloat.x, XMVectorGetY(position) + 1.5f, positionFloat.z));

		XMVECTOR objectFront = object->getTransform().getFrontVector();
		XMVECTOR objectRight = object->getTransform().getRightVector();

		XMVECTOR tangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectRight), getWaterHeightAt(positionFloat.x + XMVectorGetX(objectRight), positionFloat.z + XMVectorGetZ(objectRight)), XMVectorGetZ(objectRight), 0.0f) - position);
		XMVECTOR bitangent = XMVector3Normalize(XMVectorSet(XMVectorGetX(objectFront), getWaterHeightAt(positionFloat.x + XMVectorGetX(objectFront), positionFloat.z + XMVectorGetZ(objectFront)), XMVectorGetZ(objectFront), 0.0f) - position);

		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(bitangent, tangent));

		XMMATRIX rotationMatrix =  XMMATRIX(tangent, normal, bitangent, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		rotationMatrix.r[0].m128_f32[3] = 0.0f;
		rotationMatrix.r[1].m128_f32[3] = 0.0f;
		rotationMatrix.r[2].m128_f32[3] = 0.0f;

		object->getTransform().setOrientationRotationMatrix(rotationMatrix);
	}
}