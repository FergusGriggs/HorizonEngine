
//Function implementations for the Graphics class

#include "graphics_handler.h"

#include <shobjidl.h>

#include "../user_config.h"

#define _SOLUTIONDIR R"($(SolutionDir))"

namespace hrzn::gfx
{
	bool GraphicsHandler::initialize(HWND hwnd)
	{
		m_fpsTimer.start();

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

		m_geometryBuffer.initialise(m_device.Get());

		createRenderPassConfigs();

		//INIT IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());
		ImGui::StyleColorsDark();

		return true;
	}

	bool GraphicsHandler::initializeScene(scene::SceneManager& sceneManager)
	{
		try
		{
			//CREATE CONSTANT BUFFERS
			HRESULT hr = m_defaultVertexShaderCB.initialize(m_device.Get(), m_deviceContext.Get());

			hr = m_waterVertexShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'waterVertexShader' constant buffer.");
			m_waterVertexShaderCB.m_data.m_waveCount = sceneManager.getSceneConfig().getOceanConfig().m_waveCount;
			m_waterVertexShaderCB.m_data.m_waveScale = sceneManager.getSceneConfig().getOceanConfig().m_waveScale;//14.3f
			m_waterVertexShaderCB.m_data.m_wavePeriod = sceneManager.getSceneConfig().getOceanConfig().m_wavePeriod;//50.5f
			m_waterVertexShaderCB.m_data.m_waveSpeed = sceneManager.getSceneConfig().getOceanConfig().m_waveSpeed;//25.0f
			m_waterVertexShaderCB.m_data.m_waveSeed = sceneManager.getSceneConfig().getOceanConfig().m_waveSeed;
			m_waterVertexShaderCB.m_data.m_waveScaleMultiplier = sceneManager.getSceneConfig().getOceanConfig().m_waveScaleMultiplier;
			m_waterVertexShaderCB.m_data.m_iscolateWaveNum = -1;

			hr = m_waterPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'waterPixelShader' constant buffer.");
			m_waterPixelShaderCB.m_data.m_waveCount = m_waterVertexShaderCB.m_data.m_waveCount;
			m_waterPixelShaderCB.m_data.m_waveScale = m_waterVertexShaderCB.m_data.m_waveScale;
			m_waterPixelShaderCB.m_data.m_wavePeriod = m_waterVertexShaderCB.m_data.m_wavePeriod;
			m_waterPixelShaderCB.m_data.m_waveSpeed = m_waterVertexShaderCB.m_data.m_waveSpeed;
			m_waterPixelShaderCB.m_data.m_waveSeed = m_waterVertexShaderCB.m_data.m_waveSeed;
			m_waterPixelShaderCB.m_data.m_waveScaleMultiplier = m_waterVertexShaderCB.m_data.m_waveScaleMultiplier;
			m_waterPixelShaderCB.m_data.m_iscolateWaveNum = m_waterVertexShaderCB.m_data.m_iscolateWaveNum;
			m_waterPixelShaderCB.m_data.m_foamStart = sceneManager.getSceneConfig().getOceanConfig().m_foamStart;
			m_waterPixelShaderCB.m_data.m_colourChangeStart = sceneManager.getSceneConfig().getOceanConfig().m_colourChangeStart;//1.123f

			hr = m_lightingPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'pixelShader' constant buffer.");
			m_lightingPixelShaderCB.m_data.m_useNormalMapping = true;
			m_lightingPixelShaderCB.m_data.m_useParallaxOcclusionMapping = true;
			m_lightingPixelShaderCB.m_data.m_selfShadowing = true;
			m_lightingPixelShaderCB.m_data.m_roughnessMapping = true;
			m_lightingPixelShaderCB.m_data.m_depthScale = 0.05f;
			m_useVSync = false;

			hr = m_noLightPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'noLightPixelShader' constant buffer.");

			hr = m_atmosphericPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'noLightPixelShader' constant buffer.");
			m_atmosphericPixelShaderCB.m_data.m_sunSize = sceneManager.getSceneConfig().getAtmosphereConfig().m_sunSize;
			m_atmosphericPixelShaderCB.m_data.m_density = sceneManager.getSceneConfig().getAtmosphereConfig().m_density;
			m_atmosphericPixelShaderCB.m_data.m_multiScatterPhase = sceneManager.getSceneConfig().getAtmosphereConfig().m_multiScatterPhase;
			m_atmosphericPixelShaderCB.m_data.m_anisotropicIntensity = sceneManager.getSceneConfig().getAtmosphereConfig().m_anisotropicIntensity;
			m_atmosphericPixelShaderCB.m_data.m_zenithOffset = sceneManager.getSceneConfig().getAtmosphereConfig().m_zenithOffset;
			m_atmosphericPixelShaderCB.m_data.m_nightDensity = sceneManager.getSceneConfig().getAtmosphereConfig().m_nightDensity;
			m_atmosphericPixelShaderCB.m_data.m_nightZenithYClamp = sceneManager.getSceneConfig().getAtmosphereConfig().m_nightZenithYClamp;

			hr = m_cloudsPixelShaderCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'cloudsPixelShader' constant buffer."); //                                                           Fluffy 1 // Fluffy 2 // Bulky bois
			m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionThroughClouds = sceneManager.getSceneConfig().getCloudConfig().m_lightAbsorbtionThroughClouds; // 0.084f      0.084f      0.338f
			m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionTowardsSun = sceneManager.getSceneConfig().getCloudConfig().m_lightAbsorbtionTowardsSun; //       0.273f      0.392f      0.559f
			m_cloudsPixelShaderCB.m_data.m_phaseFactor = sceneManager.getSceneConfig().getCloudConfig().m_phaseFactor; //                                   0.208f      0.266f      0.428f
			m_cloudsPixelShaderCB.m_data.m_darknessThreshold = sceneManager.getSceneConfig().getCloudConfig().m_darknessThreshold; //                       0.09f       0.073f      0.09f
			m_cloudsPixelShaderCB.m_data.m_cloudCoverage = sceneManager.getSceneConfig().getCloudConfig().m_cloudCoverage; //                               0.465f      0.497f      0.446f
			m_cloudsPixelShaderCB.m_data.m_cloudSpeed = sceneManager.getSceneConfig().getCloudConfig().m_cloudSpeed;
			m_cloudsPixelShaderCB.m_data.m_numSteps = sceneManager.getSceneConfig().getCloudConfig().m_numSteps;
			m_cloudsPixelShaderCB.m_data.m_stepSize = sceneManager.getSceneConfig().getCloudConfig().m_stepSize;
			m_cloudsPixelShaderCB.m_data.m_cloudHeight = sceneManager.getSceneConfig().getCloudConfig().m_cloudHeight;

			// Load axis models
			m_axisTranslateModel = ResourceManager::it().getModelPtr("res/models/axis/fancy_translate.obj");
			m_axisRotateModel = ResourceManager::it().getModelPtr("res/models/axis/rotate2.obj");

			m_springModel = ResourceManager::it().getModelPtr("res/models/spring.obj");

			m_quadModel = ResourceManager::it().getModelPtr("res/models/misc/screen_quad.obj");

			m_defaultDiffuseTexture = new Texture(m_device.Get(), "res/textures/scales/diffuse.jpg", aiTextureType::aiTextureType_DIFFUSE);
			m_defaultSpecularTexture = new Texture(m_device.Get(), "res/textures/scales/specular.jpg", aiTextureType::aiTextureType_DIFFUSE);
			m_defaultNormalTexture = new Texture(m_device.Get(), "res/textures/scales/normal.jpg", aiTextureType::aiTextureType_DIFFUSE);

			m_highlightDiffuseTexture = new Texture(m_device.Get(), "res/textures/hrzn_statue/diffuse.jpg", aiTextureType::aiTextureType_DIFFUSE);
			m_highlightSpecularTexture = new Texture(m_device.Get(), "res/textures/hrzn_statue/roughness.jpg", aiTextureType::aiTextureType_DIFFUSE);
			m_highlightNormalTexture = new Texture(m_device.Get(), "res/textures/hrzn_statue/normal.png", aiTextureType::aiTextureType_DIFFUSE);
		}
		catch (utils::COMException& exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}

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
		D3D11_INPUT_ELEMENT_DESC defaultLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		UINT defaultNumElements = ARRAYSIZE(defaultLayout);

		D3D11_INPUT_ELEMENT_DESC quadLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		UINT quadNumElements = ARRAYSIZE(quadLayout);

		//Initialise vertex shaders
		if (!m_defaultVertexShader.initialize(m_device, shaderFolder + L"defaultVertexShader.cso", defaultLayout, defaultNumElements))
		{
			return false;
		}

		if (!m_quadVertexShader.initialize(m_device, shaderFolder + L"quadVertexShader.cso", quadLayout, quadNumElements))
		{
			return false;
		}

		if (!m_waterVertexShader.initialize(m_device, shaderFolder + L"waterVertexShader.cso", defaultLayout, defaultNumElements))
		{
			return false;
		}

		//Initialise pixel shaders
		if (!m_lightingPixelShader.initialize(m_device, shaderFolder + L"lightingPixelShader.cso"))
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

		if (!m_gBufferPixelShader.initialize(m_device, shaderFolder + L"gBufferPixelShader.cso"))
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

		return true;
	}

	void GraphicsHandler::render(scene::SceneManager& sceneManager)
	{
		float blackColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		// Render geometry from main camera to the GBuffer
		m_defaultRenderConfig.m_viewMatrix = sceneManager.getActiveCamera().getViewMatrix();
		m_defaultRenderConfig.m_projectionMatrix = sceneManager.getActiveCamera().getProjectionMatrix();

		// Set rasterizer state
		if (m_useWireframe)
		{
			m_defaultRenderConfig.m_rasterizerState = m_wireframeRasterizerState.Get();
		}
		else
		{
			m_defaultRenderConfig.m_rasterizerState = m_regularRasterizerState.Get();
		}

		renderSceneObjects(sceneManager, m_defaultRenderConfig);

		m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), blackColour);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_deviceContext->IASetInputLayout(m_quadVertexShader.getInputLayout());
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_deviceContext->VSSetShader(m_quadVertexShader.getShader(), NULL, 0);
		m_deviceContext->PSSetShader(m_lightingPixelShader.getShader(), NULL, 0);

		// Point light shader variables
		const auto& pointLights = sceneManager.getPointLights();
		size_t numPointLights = pointLights.size();
		for (size_t i = 0; i < numPointLights; ++i)
		{
			pointLights[i]->updateShaderVariables(m_lightingPixelShaderCB, i);
		}

		// Spot light shader variables
		const auto& spotLights = sceneManager.getSpotLights();
		size_t numSpotLights = spotLights.size();
		for (size_t i = 0; i < numSpotLights; ++i)
		{
			spotLights[i]->updateShaderVariables(m_lightingPixelShaderCB, i);
		}

		// General shader variables
		m_lightingPixelShaderCB.m_data.m_numPointLights = static_cast<int>(numPointLights);
		m_lightingPixelShaderCB.m_data.m_numSpotLights = static_cast<int>(numSpotLights);

		m_lightingPixelShaderCB.m_data.m_cameraPosition = sceneManager.getActiveCamera().getTransform().getPositionFloat3();

		m_lightingPixelShaderCB.m_data.m_objectMaterial.m_shininess = 4.0f;
		m_lightingPixelShaderCB.m_data.m_objectMaterial.m_specularity = 0.75f;

		m_lightingPixelShaderCB.mapToGPU();
		m_deviceContext->PSSetConstantBuffers(0, 1, m_lightingPixelShaderCB.getAddressOf());

		m_quadModel->drawRaw();

		// Update FPS timer
		m_fpsCounter++;

		if (m_fpsTimer.getMicrosecondsElapsed() > 1000000)
		{
			m_fpsString = "FPS: " + std::to_string(m_fpsCounter);
			m_fpsCounter = 0;
			m_fpsTimer.restart();
		}

		// Render UI (not ImGui)
		m_spriteBatch->Begin();

		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(1.0f, 0.5f), DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));

		if (sceneManager.objectIsSelected())
		{
			// Stop text being drawn behind camera
			if (XMVectorGetX(XMVector3Dot(sceneManager.getSelectedObject()->getTransform().getPositionVector() - sceneManager.getActiveCamera().getTransform().getPositionVector(), sceneManager.getActiveCamera().getTransform().getFrontVector())) >= 0.0f)
			{
				XMFLOAT2 screenNDC = sceneManager.getActiveCamera().getNDCFrom3DPos(sceneManager.getSelectedObject()->getTransform().getPositionVector() + XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f)); // XMFLOAT2(-0.5,0.5);
				XMFLOAT2 screenPos = DirectX::XMFLOAT2((screenNDC.x * 0.5f + 0.5f) * UserConfig::it().getWindowWidth(), (1.0f - (screenNDC.y * 0.5f + 0.5f)) * UserConfig::it().getWindowHeight());
				XMVECTOR size = m_spriteFont->MeasureString(utils::string_helpers::stringToWide(sceneManager.getSelectedObject()->getLabel()).c_str());
				screenPos.x -= XMVectorGetX(size) * 0.5f;
				screenPos.y -= XMVectorGetY(size) * 0.5f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(sceneManager.getSelectedObject()->getLabel()).c_str(), screenPos, DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
				screenPos.y -= 0.5f;
				screenPos.x -= 1.0f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(sceneManager.getSelectedObject()->getLabel()).c_str(), screenPos, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			}
		}

		m_spriteBatch->End();

		ImGui::Render();

		// Render imgui
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present image
		m_swapChain->Present(m_useVSync, NULL);// using vsync

		// Render scene depth from directional light's perspective
		//renderSceneObjects(sceneManager, );

		// Work out scene ambient occulsion using depth

		// Shade objects using the gbuffer
		
		// Do postprocessing
	}

	void GraphicsHandler::renderSceneObjects(scene::SceneManager& sceneManager, const RenderPassConfig& renderPassConfig)
	{
		m_deviceContext->RSSetViewports(1, &renderPassConfig.m_viewport);

		m_deviceContext->RSSetState(renderPassConfig.m_rasterizerState);
		m_deviceContext->OMSetDepthStencilState(renderPassConfig.m_depthStencilState, 0);
		m_deviceContext->OMSetBlendState(renderPassConfig.m_blendState, NULL, 0xFFFFFFFF);

		sceneManager.getWritableActiveCamera().updateView();

		sceneManager.getWritableDirectionalLight().updateShaderVariables(m_lightingPixelShaderCB);

		// Clear render target views and depth stencil view
		float backgroundColour[4] = { 0.62f * sceneManager.getDirectionalLight().m_colour.x, 0.9f * sceneManager.getDirectionalLight().m_colour.y, 1.0f * sceneManager.getDirectionalLight().m_colour.z, 1.0f };

		m_deviceContext->OMSetRenderTargets(renderPassConfig.m_numRenderTargetViews, renderPassConfig.m_renderTargetViews, renderPassConfig.m_depthStencilView);
		for (int renderTargetIndex = 0; renderTargetIndex < renderPassConfig.m_numRenderTargetViews; ++renderTargetIndex)
		{
			m_deviceContext->ClearRenderTargetView(renderPassConfig.m_renderTargetViews[renderTargetIndex], backgroundColour);
		}

		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Update input assembler
		m_deviceContext->IASetInputLayout(m_defaultVertexShader.getInputLayout());
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set sampler state
		m_deviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		// Set vertex and pixel shaders
		m_deviceContext->VSSetShader(m_defaultVertexShader.getShader(), NULL, 0);
	
		UINT offset = 0;

		XMMATRIX viewProjMat = sceneManager.getActiveCamera().getViewMatrix() * sceneManager.getActiveCamera().getProjectionMatrix();

		// Draw skybox
		m_deviceContext->PSSetShader(m_atmosphericPixelShader.getShader(), NULL, 0);
		m_deviceContext->PSSetConstantBuffers(0, 1, m_atmosphericPixelShaderCB.getAddressOf());

		m_atmosphericPixelShaderCB.m_data.m_cameraPosition = sceneManager.getActiveCamera().getTransform().getPositionFloat3();

		m_atmosphericPixelShaderCB.m_data.m_sunDirection = sceneManager.getDirectionalLight().getTransform().getBackFloat3();
			
		m_atmosphericPixelShaderCB.mapToGPU();

		sceneManager.getWritableSkybox().getWritableTransform().setPosition(sceneManager.getActiveCamera().getTransform().getPositionFloat3());
		sceneManager.getSkybox().draw(viewProjMat, &m_defaultVertexShaderCB);

		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Regular objects
		m_deviceContext->PSSetConstantBuffers(0, 1, m_lightingPixelShaderCB.getAddressOf());
		m_deviceContext->PSSetShader(m_gBufferPixelShader.getShader(), NULL, 0);

		m_deviceContext->PSSetShaderResources(0, 1, m_defaultDiffuseTexture->getTextureResourceViewAddress());
		m_deviceContext->PSSetShaderResources(1, 1, m_defaultSpecularTexture->getTextureResourceViewAddress());
		m_deviceContext->PSSetShaderResources(2, 1, m_defaultNormalTexture->getTextureResourceViewAddress());

		const auto& renderables = sceneManager.getRenderables();
		size_t numRenderables = renderables.size();
		for (size_t i = 0; i < numRenderables; ++i)
		{
			entity::RenderableGameObject* renderable = renderables[i];

			if (auto* light = dynamic_cast<entity::LightGameObject*>(renderable))
			{
				m_noLightPixelShaderCB.m_data.m_colour = light->getColour();

				m_noLightPixelShaderCB.mapToGPU();

				m_deviceContext->PSSetConstantBuffers(0, 1, m_noLightPixelShaderCB.getAddressOf());

				m_deviceContext->PSSetShader(m_noLightPixelShader.getShader(), NULL, 0);

				renderable->draw(viewProjMat, &m_defaultVertexShaderCB);

				m_deviceContext->PSSetShader(m_lightingPixelShader.getShader(), NULL, 0);
				m_deviceContext->PSSetConstantBuffers(0, 1, m_lightingPixelShaderCB.getAddressOf());
			}
			else
			{
				renderable->draw(viewProjMat, &m_defaultVertexShaderCB);
			}
		}

		// Draw particles
		sceneManager.getParticleSystem().drawParticles(viewProjMat, &m_defaultVertexShaderCB);

		// Draw Springs
		XMMATRIX springModelMatrix;

		const auto& springs = sceneManager.getSprings();
		for (int i = 0; i < springs.size(); ++i)
		{
			XMVECTOR springStart = springs[i]->getSpringStart()->getTransformReference()->getPositionVector();
			XMVECTOR springEnd = springs[i]->getSpringEnd()->getTransformReference()->getPositionVector();

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
			}
			
			m_springModel->draw(springModelMatrix, viewProjMat, &m_defaultVertexShaderCB);
		}

		// Draw ocean
		if (sceneManager.getSceneConfig().getOceanConfig().m_enabled)
		{
			m_deviceContext->VSSetShader(m_waterVertexShader.getShader(), NULL, 0);
			m_deviceContext->PSSetShader(m_waterPixelShader.getShader(), NULL, 0);

			m_waterVertexShaderCB.mapToGPU();

			// Put the centre a bit in front of the camera where the best fidelity is in the mesh
			float fovDistMod = (1.0f - (fminf(70.0f, sceneManager.getActiveCamera().getFOV()) / 70.0f)) * 150.0f;
			XMVECTOR oceanPosition = sceneManager.getActiveCamera().getTransform().getPositionVector() + sceneManager.getActiveCamera().getTransform().getFrontVector() * (abs(sceneManager.getActiveCamera().getTransform().getPositionFloat3().y) + 30.0f + fovDistMod) * 1.2f;

			sceneManager.getWritableOcean().getWritableTransform().setPosition(XMVectorGetX(oceanPosition), sceneManager.getOcean().getTransform().getPositionFloat3().y, XMVectorGetZ(oceanPosition));

			//float scaleMod = fmaxf(1.0f, m_camera.getTransform().getPositionFloat3().y * 0.01f);
			//m_ocean.setScale(XMFLOAT3(scaleMod, scaleMod, scaleMod));

			m_waterPixelShaderCB.m_data.m_cameraPosition = sceneManager.getActiveCamera().getTransform().getPositionFloat3();

			XMStoreFloat3(&m_waterPixelShaderCB.m_data.m_lightDirection, sceneManager.getDirectionalLight().getTransform().getFrontVector());

			m_waterPixelShaderCB.mapToGPU();

			m_deviceContext->PSSetConstantBuffers(0, 1, m_waterPixelShaderCB.getAddressOf());
			m_deviceContext->PSSetShaderResources(0, 1, m_noiseTextureShaderResourceView.GetAddressOf());

			sceneManager.getOcean().draw(viewProjMat, &m_waterVertexShaderCB, false);
		}

		// Draw clouds
		if (sceneManager.getSceneConfig().getCloudConfig().m_enabled)
		{
			m_deviceContext->VSSetShader(m_defaultVertexShader.getShader(), NULL, 0);
			m_deviceContext->PSSetShader(m_cloudsPixelShader.getShader(), NULL, 0);

			XMFLOAT3 cameraPosFloat = sceneManager.getActiveCamera().getTransform().getPositionFloat3();
			m_cloudsPixelShaderCB.m_data.m_cameraPosition = cameraPosFloat;

			XMStoreFloat3(&m_cloudsPixelShaderCB.m_data.m_lightDirection, sceneManager.getDirectionalLight().getTransform().getFrontVector());
			m_cloudsPixelShaderCB.mapToGPU();

			m_deviceContext->PSSetConstantBuffers(0, 1, m_cloudsPixelShaderCB.getAddressOf());
			m_deviceContext->PSSetShaderResources(0, 1, m_noiseTextureShaderResourceView.GetAddressOf());

			sceneManager.getWritableClouds().getWritableTransform().setPosition(cameraPosFloat.x, sceneManager.getClouds().getTransform().getPositionFloat3().y, cameraPosFloat.z);

			sceneManager.getClouds().draw(viewProjMat, &m_defaultVertexShaderCB, false);
		}
	
		// Draw axis
		if (sceneManager.objectIsSelected())
		{
			m_deviceContext->PSSetShader(m_noLightPixelShader.getShader(), NULL, 0);
			m_deviceContext->PSSetConstantBuffers(0, 1, m_noLightPixelShaderCB.getAddressOf());
			m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			m_noLightPixelShaderCB.m_data.m_colour = XMFLOAT3(1.0f, 1.0f, 1.0f);
			m_noLightPixelShaderCB.m_data.m_cameraPos = sceneManager.getActiveCamera().getTransform().getPositionFloat3();
			m_noLightPixelShaderCB.mapToGPU();
			drawAxisForObject(*(sceneManager.getSelectedObject()), viewProjMat, sceneManager);
		}
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
		textureDesc.Format = DXGI_FORMAT_R8_SNORM;//DXGI_FORMAT_R8_SNORM
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

		// Unbind the UAV
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

	void GraphicsHandler::createRenderPassConfigs()
	{
		// Default render config
		m_defaultRenderConfig.m_viewport = m_defaultViewport;

		m_defaultRenderConfig.m_renderTargetViews = m_geometryBuffer.m_renderTargetViews;
		m_defaultRenderConfig.m_numRenderTargetViews = 4;

		m_defaultRenderConfig.m_depthStencilView = m_geometryBuffer.m_depthStencil.m_depthStencilView.Get();

		m_defaultRenderConfig.m_depthStencilState = m_depthStencilState.Get();
		m_defaultRenderConfig.m_rasterizerState = m_regularRasterizerState.Get();
		m_defaultRenderConfig.m_blendState = m_blendState.Get();

		m_defaultRenderConfig.m_viewMatrix = XMMatrixIdentity();
		m_defaultRenderConfig.m_projectionMatrix = XMMatrixIdentity();

		m_defaultRenderConfig.m_highestLOD = 0;
	}

	void GraphicsHandler::updateImGui(scene::SceneManager& sceneManager)
	{
		// Start new ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//New object menu
		/*if (m_newObjectMenuOpen)
		{
			if (ImGui::Button("Create Object"))
			{

			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_newObjectMenuOpen = false;
			}
		}*/

		// Selected object menu
		if (sceneManager.objectIsSelected())
		{
			entity::GameObject::Type selectedObjectType = sceneManager.getSelectedObject()->getType();

			ImGui::Begin("Game Object Settings");

			ImGui::Text(("Label: " + sceneManager.getSelectedObject()->getLabel()).c_str());

			ImGui::InputText("Object Label", &sceneManager.getWritableSelectedObject()->getLabelPtr()->at(0), 20);

			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::TreePush();

				if (ImGui::CollapsingHeader("Translation"))
				{
					ImGui::TreePush();

					sceneManager.getWritableSelectedObject()->getWritableTransform().editPositionImGui();

					if (ImGui::Button("Edit Translation"))
					{
						sceneManager.setAxisEditState(scene::AxisEditState::eEditTranslate);
					}

					ImGui::TreePop();
				}

				if (ImGui::CollapsingHeader("Orientation"))
				{
					ImGui::TreePush();

					sceneManager.getWritableSelectedObject()->getWritableTransform().displayOrientationImGui();

					if (ImGui::Button("Edit Orientation"))
					{
						sceneManager.setAxisEditState(scene::AxisEditState::eEditRotate);
					}

					ImGui::SameLine();

					if (ImGui::Button("Reset Orientation"))
					{
						sceneManager.getWritableSelectedObject()->getWritableTransform().setOrientationQuaternion(XMQuaternionIdentity());
					}

					if (ImGui::CollapsingHeader("Axis Rotate"))
					{
						ImGui::TreePush();

						static XMFLOAT3 axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
						static float angle = 1.5707963f;

						ImGui::DragFloat3("Axis", &axis.x, 0.1f, -1.0f, 1.0f);
						ImGui::SliderFloat("Angle", &angle, 3.141592f, -3.141592f);

						if (ImGui::Button("Normalise Axis"))
						{
							XMVECTOR axisVec = XMLoadFloat3(&axis);
							axisVec = XMVector3Normalize(axisVec);
							XMStoreFloat3(&axis, axisVec);
						}
						ImGui::SameLine();
						if (ImGui::Button("Rotate about axis by angle"))
						{
							sceneManager.getWritableSelectedObject()->getWritableTransform().rotateUsingAxis(XMLoadFloat3(&axis), angle);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				// If it's a renderable, allow scaling
				if (selectedObjectType == entity::GameObject::Type::eRenderable)
				{
					if (ImGui::CollapsingHeader("Scaling"))
					{
						ImGui::TreePush();

						XMFLOAT3 scale = sceneManager.getSelectedObject()->getScale();
						ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.1f, 10.0f);
						sceneManager.getWritableSelectedObject()->setScale(scale);

						if (ImGui::Button("Edit Scaling"))
						{
							sceneManager.setAxisEditState(scene::AxisEditState::eEditScale);
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			// If it's a light, allow light property changing and moving to camera
			if (selectedObjectType == entity::GameObject::Type::ePointLight || selectedObjectType == entity::GameObject::Type::eSpotLight)
			{
				if (ImGui::CollapsingHeader("Light Settings"))
				{
					ImGui::TreePush();

					entity::LightGameObject* lightObj = reinterpret_cast<entity::LightGameObject*>(sceneManager.getWritableSelectedObject());
					ImGui::ColorEdit3("Colour", &(lightObj->m_colour.x));

					entity::PointLightGameObject* pointLightObj = reinterpret_cast<entity::PointLightGameObject*>(lightObj);
					ImGui::SliderFloat("Att. Const.", pointLightObj->getAttenuationConstantPtr(), 0.05f, 1.0f);
					ImGui::SliderFloat("Att. Lin.", pointLightObj->getAttenuationLinearPtr(), 0.0f, 0.2f);
					ImGui::SliderFloat("Att. Quad.", pointLightObj->getAttenuationQuadraticPtr(), 0.0f, 0.05f);

					if (selectedObjectType == entity::GameObject::Type::eSpotLight)
					{
						entity::SpotLightGameObject* spotLightObj = reinterpret_cast<entity::SpotLightGameObject*>(lightObj);
						ImGui::SliderFloat("Inn. Cut.", spotLightObj->getInnerCutoffPtr(), 0.0f, 90.0f);
						ImGui::SliderFloat("Out. Cut.", spotLightObj->getOuterCutoffPtr(), 0.0f, 90.0f);
					}

					if (ImGui::Button("Move to Camera"))
					{
						XMVECTOR lightPosition = sceneManager.getActiveCamera().getTransform().getPositionVector();
						lightPosition += sceneManager.getActiveCamera().getTransform().getFrontVector();
						lightObj->getWritableTransform().setPosition(lightPosition);
						lightObj->getWritableTransform().copyOrientationFrom(sceneManager.getActiveCamera().getTransform());
						lightObj->setFollowingObjectTrack(false);
					}

					ImGui::TreePop();
				}
			}

			if (selectedObjectType == entity::GameObject::Type::ePhysics)
			{
				if (ImGui::CollapsingHeader("Physics Settings"))
				{
					ImGui::TreePush();

					entity::PhysicsGameObject* object = dynamic_cast<entity::PhysicsGameObject*>(sceneManager.getWritableSelectedObject());
					ImGui::Checkbox("Static", object->getRigidBody()->isStaticPtr());
					if (ImGui::Button("Apply Upwards Thrust"))
					{
						object->getRigidBody()->addThrust(XMVectorSet(0.0f, 850.0f, 0.0f, 0.0f), 0.5f);
					}
					if (ImGui::Button("Apply Forward Thrust"))
					{
						object->getRigidBody()->addThrust(XMVector3Normalize(object->getTransform().getPositionVector() - sceneManager.getActiveCamera().getTransform().getPositionVector()) * 800.0f, 0.5f);
					}
					if (ImGui::Button("Apply Torque"))
					{
						XMVECTOR worldPos = (object->getTransform().getPositionVector() + sceneManager.getActiveCamera().getTransform().getPositionVector()) * 0.5f;
						object->getRigidBody()->addTorque(worldPos - object->getTransform().getPositionVector(), XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f));
					}
					if (ImGui::Button("Apply Split Force"))
					{
						XMVECTOR worldPos = sceneManager.getActiveCamera().getTransform().getPositionVector();
						object->getRigidBody()->addForceSplit(worldPos, sceneManager.getActiveCamera().getTransform().getFrontVector() * 10000.0f);
					}

					ImGui::TreePop();
				}
			}

			if (ImGui::CollapsingHeader("Misc"))
			{
				ImGui::TreePush();

				if (sceneManager.getSelectedObject()->hasObjectTrack())
				{
					bool followingTrack = sceneManager.getSelectedObject()->isFollowingObjectTrack();
					ImGui::Checkbox("Follow Track", &followingTrack);
					sceneManager.getWritableSelectedObject()->setFollowingObjectTrack(followingTrack);
				}

				bool floatingObject = sceneManager.getSelectedObject()->getFloating();
				ImGui::Checkbox("Float Object", &floatingObject);
				sceneManager.getWritableSelectedObject()->setFloating(floatingObject);

				if (sceneManager.getSelectedObject()->isFollowingObject())
				{
					if (ImGui::Button("Stop Following Object"))
					{
						sceneManager.getWritableSelectedObject()->setObjectToFollow(nullptr);
					}
				}
				else
				{
					if (ImGui::TreeNode("Followable Objects"))
					{
						auto mapIterator = sceneManager.getObjectMap().begin();
						while (mapIterator != sceneManager.getObjectMap().end())
						{
							std::string label = "Follow " + mapIterator->second->getLabel();

							if (ImGui::Button(label.c_str()))
							{
								sceneManager.getWritableSelectedObject()->setObjectToFollow(mapIterator->second);
							}

							mapIterator++;
						}
						ImGui::TreePop();
					}
				}

				size_t numRelativeCameras = sceneManager.getSelectedObject()->getRelativePositions().size();
				if (numRelativeCameras != 0)
				{
					ImGui::Text("Relative Cameras");
				}
				for (size_t i = 0; i < numRelativeCameras; ++i)
				{
					if (ImGui::Button(("Camera " + std::to_string(i + 1)).c_str()))
					{
						sceneManager.getWritableActiveCamera().setRelativeObject(sceneManager.getSelectedObject(), sceneManager.getSelectedObject()->getRelativePositions()[i]);
						sceneManager.getWritableActiveCamera().setFollowingObjectTrack(false);
					}
					if (i < numRelativeCameras - 1)
					{
						ImGui::SameLine();
					}
				}

				entity::GameObjectController* controller = sceneManager.getWritableSelectedObject()->getWritableController();
				if (controller != nullptr)
				{
					ImGui::Checkbox("Control Object", controller->isActivePtr());
				}

				if (ImGui::Button("Delete Object"))
				{
					sceneManager.removeGameObject(sceneManager.getSelectedObject()->getLabel());
					sceneManager.setSelectedObject(nullptr);
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Close"))
			{
				sceneManager.setSelectedObject(nullptr);
			}
		
			ImGui::End();
		}

		ImGui::Begin("Object Selection");
	
		auto mapIterator = sceneManager.getObjectMap().begin();
		while (mapIterator != sceneManager.getObjectMap().end())
		{
			if (entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second))
			{
				std::string label = gameObject->getLabel();

				if (sceneManager.getSelectedObject() == gameObject)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ("> " + label).c_str());
				}
				else if (ImGui::MenuItem(label.c_str()))
				{
					sceneManager.setSelectedObject(gameObject);
				}
			}

			mapIterator++;
		}

		ImGui::End();

		ImGui::Begin("Scene Settings");
	
		/*if (ImGui::Button("Save Scene"))
		{
			sceneManager.saveScene(sceneManager.getSceneName().c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene"))
		{
			sceneManager.loadScene("test");
		}
		ImGui::SameLine();
		if (ImGui::Button("Unload Scene"))
		{
			sceneManager.unloadScene();
		}*/
		
		//DIRECTIONAL LIGHT COLOUR
		ImGui::ColorEdit3("Dir Light Colour", &(sceneManager.getWritableDirectionalLight().m_colour.x));
		m_cloudsPixelShaderCB.m_data.m_lightColour = sceneManager.getDirectionalLight().m_colour;
		m_waterPixelShaderCB.m_data.m_lightColour = sceneManager.getWritableDirectionalLight().m_colour;

		ImGui::Checkbox("Use VSync", &m_useVSync);

		ImGui::SameLine();

		ImGui::Checkbox("Day/Night Cycle", sceneManager.getDayNightCyclePtr());

		ImGui::SameLine();

		ImGui::Checkbox("Pause", sceneManager.getPausedPtr());
		ImGui::SliderFloat("Day Progress", sceneManager.getDayProgressPtr(), 0.0f, 1.0f);

		if (ImGui::CollapsingHeader("Object Spawning"))
		{
			ImGui::TreePush();

			static char label[20] = "";
			static char path[50] = "city/wall/frames/left/r.obj";
			ImGui::InputText("New Object Label", &label[0], 20);
			ImGui::InputText("New Object Path", &path[0], 50);

			if (ImGui::Button("Spawn Object"))
			{
				IFileOpenDialog* pFileOpen;

				// Create the FileOpenDialog object.
				HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr))
				{
					//LPCWSTR modelPath = utils::string_helpers::stringToWide(_SOLUTIONDIR).c_str();
					LPCWSTR modelPath = L"C:\\Users\\g012090i\\source\\repos\\FergusGriggs\\HorizonEngine\\HorizonEngine\\res\\models\\";

					Microsoft::WRL::ComPtr<IShellItem> location;
					hr = SHCreateItemFromParsingName(modelPath, nullptr, IID_PPV_ARGS(&location));

					pFileOpen->SetFolder(location.Get());

					// Show the Open dialog box.
					hr = pFileOpen->Show(NULL);

					// Get the file name from the dialog box.
					if (SUCCEEDED(hr))
					{
						IShellItem* pItem;
						hr = pFileOpen->GetResult(&pItem);
						if (SUCCEEDED(hr))
						{
							PWSTR pszFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

							// Display the file name to the user.
							if (SUCCEEDED(hr))
							{
								MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
								CoTaskMemFree(pszFilePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}

				std::string labelChosen = label;
				if (labelChosen != "" && sceneManager.getObjectMap().find(labelChosen) == sceneManager.getObjectMap().end())
				{
					entity::RenderableGameObject* newObject = new entity::RenderableGameObject();
					std::string pathstr = path;
					newObject->initialize(label, "res/models/environment/meshes/" + pathstr);

					newObject->getWritableTransform().setPosition(sceneManager.getActiveCamera().getTransform().getPositionVector() + sceneManager.getActiveCamera().getTransform().getFrontVector() * 5.0f);

					sceneManager.addGameObject(newObject);
					sceneManager.setSelectedObject(newObject);
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::CollapsingHeader("Render Options"))
		{
			ImGui::TreePush();

			// Normal Mapping
			bool useNormalMapping = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_useNormalMapping);
			ImGui::Checkbox("Normal Mapping", &useNormalMapping);
			m_lightingPixelShaderCB.m_data.m_useNormalMapping = useNormalMapping;

			ImGui::SameLine();

			// PO Mapping
			bool useParallaxOcclusionMapping = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_useParallaxOcclusionMapping);
			ImGui::Checkbox("PO Mapping", &useParallaxOcclusionMapping);
			m_lightingPixelShaderCB.m_data.m_useParallaxOcclusionMapping = useParallaxOcclusionMapping;

			ImGui::SameLine();

			// Self shadowing
			bool selfShadowing = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_selfShadowing);
			ImGui::Checkbox("Self Shadowing", &selfShadowing);
			m_lightingPixelShaderCB.m_data.m_selfShadowing = selfShadowing;

			// Roughness Mapping
			bool roughnessMapping = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_roughnessMapping);
			ImGui::Checkbox("Roughness Mapping", &roughnessMapping);
			m_lightingPixelShaderCB.m_data.m_roughnessMapping = roughnessMapping;

			ImGui::SameLine();

			ImGui::Checkbox("Wireframe", &m_useWireframe);

			ImGui::SameLine();

			// Show Normals
			bool showWorldNormals = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_showWorldNormals);
			ImGui::Checkbox("Normals", &showWorldNormals);
			m_lightingPixelShaderCB.m_data.m_showWorldNormals = showWorldNormals;

			ImGui::SameLine();

			// Show UVs
			bool showUVs = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_showUVs);
			ImGui::Checkbox("Show UVs", &showUVs);
			m_lightingPixelShaderCB.m_data.m_showUVs = showUVs;

			// Cull back normals
			bool cullBackNormals = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_cullBackNormals);
			ImGui::Checkbox("Cull Back Normals", &cullBackNormals);
			m_lightingPixelShaderCB.m_data.m_cullBackNormals = cullBackNormals;

			ImGui::SameLine();

			// Gamma correction
			bool gammaCorrection = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_gammaCorrection);
			ImGui::Checkbox("Gamma Corr", &gammaCorrection);
			m_lightingPixelShaderCB.m_data.m_gammaCorrection = gammaCorrection;

			ImGui::SameLine();

			// Misc toggle A
			bool miscToggleA = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_miscToggleA);
			ImGui::Checkbox("Misc A", &miscToggleA);
			m_lightingPixelShaderCB.m_data.m_miscToggleA = miscToggleA;

			ImGui::SameLine();

			// Misc toggle B
			bool miscToggleB = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_miscToggleB);
			ImGui::Checkbox("Misc B", &miscToggleB);
			m_lightingPixelShaderCB.m_data.m_miscToggleB = miscToggleB;

			// Misc toggle C
			bool miscToggleC = static_cast<bool>(m_lightingPixelShaderCB.m_data.m_miscToggleC);
			ImGui::Checkbox("Misc C", &miscToggleC);
			m_lightingPixelShaderCB.m_data.m_miscToggleC = miscToggleC;

			// Depth scale
			ImGui::DragFloat("Depth Scale", &m_lightingPixelShaderCB.m_data.m_depthScale, 0.001f, 0.0f, 0.5f);

			ImGui::TreePop();
		}
		
		if (ImGui::CollapsingHeader("Ocean Options"))
		{
			ImGui::TreePush();

			ImGui::Checkbox("Enabled", &sceneManager.getWritableSceneConfig().getWritableOceanConfig().m_enabled);

			ImGui::SliderInt("Wave Count", &m_waterVertexShaderCB.m_data.m_waveCount, 0, 50);
			ImGui::SliderFloat("Wave Scale", &m_waterVertexShaderCB.m_data.m_waveScale, 0.0f, 25.0f);
			ImGui::SliderFloat("Wave Period", &m_waterVertexShaderCB.m_data.m_wavePeriod, 0.0f, 100.0f);
			ImGui::SliderFloat("Wave Speed", &m_waterVertexShaderCB.m_data.m_waveSpeed, 0.0f, 100.0f);
			ImGui::SliderFloat("Wave Seed", &m_waterVertexShaderCB.m_data.m_waveSeed, 100.0f, 1000.0f);
			ImGui::SliderFloat("Wave Scale Multiplier", &m_waterVertexShaderCB.m_data.m_waveScaleMultiplier, 0.0f, 1.0f);
			ImGui::SliderInt("Iscolate Wave Num", &m_waterVertexShaderCB.m_data.m_iscolateWaveNum, -1, 20);

			m_waterPixelShaderCB.m_data.m_waveCount = m_waterVertexShaderCB.m_data.m_waveCount;
			m_waterPixelShaderCB.m_data.m_waveScale = m_waterVertexShaderCB.m_data.m_waveScale;
			m_waterPixelShaderCB.m_data.m_wavePeriod = m_waterVertexShaderCB.m_data.m_wavePeriod;
			m_waterPixelShaderCB.m_data.m_waveSpeed = m_waterVertexShaderCB.m_data.m_waveSpeed;
			m_waterPixelShaderCB.m_data.m_waveSeed = m_waterVertexShaderCB.m_data.m_waveSeed;
			m_waterPixelShaderCB.m_data.m_waveScaleMultiplier = m_waterVertexShaderCB.m_data.m_waveScaleMultiplier;
			m_waterPixelShaderCB.m_data.m_iscolateWaveNum = m_waterVertexShaderCB.m_data.m_iscolateWaveNum;

			ImGui::SliderFloat("Foam Start", &m_waterPixelShaderCB.m_data.m_foamStart, 0.0f, 5.0f);
			ImGui::SliderFloat("Colour Change Start", &m_waterPixelShaderCB.m_data.m_colourChangeStart, 0.0f, 2.0f);

			ImGui::TreePop();
		}

		if (ImGui::CollapsingHeader("Atmosphere Options"))
		{
			ImGui::TreePush();

			ImGui::SliderFloat("Sun Size", &m_atmosphericPixelShaderCB.m_data.m_sunSize, 10.0f, 200.0f);
			ImGui::SliderFloat("Density", &m_atmosphericPixelShaderCB.m_data.m_density, 0.0f, 3.0f);
			ImGui::SliderFloat("Multi Scatter Phase", &m_atmosphericPixelShaderCB.m_data.m_multiScatterPhase, 0.0f, 3.0f);
			ImGui::SliderFloat("Anisotropic Intensity", &m_atmosphericPixelShaderCB.m_data.m_anisotropicIntensity, -1.0f, 5.0f);
			ImGui::SliderFloat("Zenith Offset", &m_atmosphericPixelShaderCB.m_data.m_zenithOffset, -1.0f, 1.0f);
			ImGui::SliderFloat("Night Density", &m_atmosphericPixelShaderCB.m_data.m_nightDensity, 0.0f, 2.0f);
			ImGui::SliderFloat("Night Zenith Y Clamp", &m_atmosphericPixelShaderCB.m_data.m_nightZenithYClamp, 0.0f, 0.1f);

			ImGui::TreePop();
		}

		if (ImGui::CollapsingHeader("Clouds Options"))
		{
			ImGui::TreePush();

			ImGui::Checkbox("Enabled", &sceneManager.getWritableSceneConfig().getWritableCloudConfig().m_enabled);

			ImGui::SliderFloat("Absorbtion Through Clouds", &m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionThroughClouds, 0.0f, 2.0f);
			ImGui::SliderFloat("Absorbtion Towards Sun", &m_cloudsPixelShaderCB.m_data.m_lightAbsorbtionTowardsSun, 0.0f, 2.0f);
			ImGui::SliderFloat("Phase Factor", &m_cloudsPixelShaderCB.m_data.m_phaseFactor, 0.0f, 2.0f);
			ImGui::SliderFloat("Darkness Threshold", &m_cloudsPixelShaderCB.m_data.m_darknessThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Cloud Coverage", &m_cloudsPixelShaderCB.m_data.m_cloudCoverage, 0.0f, 1.0f);
			ImGui::SliderFloat("Cloud Speed", &m_cloudsPixelShaderCB.m_data.m_cloudSpeed, 0.0f, 0.25f);
			ImGui::SliderFloat("Cloud Height", &m_cloudsPixelShaderCB.m_data.m_cloudHeight, 100.0f, 2000.0f);
			ImGui::SliderInt("Num Steps", &m_cloudsPixelShaderCB.m_data.m_numSteps, 1, 100);
			ImGui::SliderFloat("Step Size", &m_cloudsPixelShaderCB.m_data.m_stepSize, 5.0f, 100.0f);

			ImGui::TreePop();
		}

		ImGui::End();


		ImGui::Begin("Particle System Settings");

		physics::ParticleSystem& particleSystem = sceneManager.getWritableParticleSystem();

		if (!particleSystem.getEmitters().empty())
		{
			static int emitterIndex = 0;
			ImGui::SliderInt("Emitter Index", &emitterIndex, 0, static_cast<int>(particleSystem.getEmitters().size()) - 1);

			physics::ParticleSystemEmitter& particleEmitter = *(particleSystem.getEmitters()[emitterIndex]);

			ImGui::Checkbox("Active", particleEmitter.getActiveWritablePtr());

			ImGui::SliderFloat3("Position", &particleEmitter.getPositionWritablePtr()->m128_f32[0], -10.0f, 10.0f);

			XMVECTOR* particleEmitterDirection = particleEmitter.getDirectionWritablePtr();
			ImGui::SliderFloat3("Direction", &particleEmitterDirection->m128_f32[0], -1.0f, 1.0f);
			*particleEmitterDirection = XMVector3Normalize(*particleEmitterDirection);

			ImGui::SliderFloat("Direction Randomness", particleEmitter.getDirectionRandomnessWritablePtr(), 0.0f, 1.0f);

			ImGui::SliderFloat("Power", particleEmitter.getPowerWritablePtr(), 0.0f, 10.0f);
			ImGui::SliderFloat("Power Randomness", particleEmitter.getPowerRandomModifierWritablePtr(), 0.0f, 1.0f);

			ImGui::SliderFloat("Max Age", particleEmitter.getMaxAgeWritablePtr(), 0.0f, 5.0f);
			ImGui::SliderFloat("Max Age Randomness", particleEmitter.getMaxAgeRandomModifierWritablePtr(), 0.0f, 1.0f);

			ImGui::SliderFloat("Spawn Delay", particleEmitter.getSpawnDelayWritablePtr(), 0.0f, 0.5f);
			ImGui::SliderFloat("Spawn Delay Rand", particleEmitter.getSpawnDelayRandomModifierWritablePtr(), 0.0f, 1.0f);
		}
		else
		{
			ImGui::Text("No emitters to show/edit");
		}
		
	
		ImGui::End();


		ImGui::Begin("Camera Settings");

		sceneManager.getWritableActiveCamera().getWritableTransform().editPositionImGui();
		sceneManager.getActiveCamera().getTransform().displayOrientationImGui();

		ImGui::NewLine();

		if (sceneManager.getActiveCamera().hasObjectTrack())
		{
			// Camera track checkbox
			bool followTrack = sceneManager.getActiveCamera().isFollowingObjectTrack();
			ImGui::Checkbox("Camera Follow Track", &followTrack);
			sceneManager.getWritableActiveCamera().setFollowingObjectTrack(followTrack);

			if (followTrack)
			{
				// Camera track delta
				float cameraTrackDelta = sceneManager.getActiveCamera().getObjectTrackDelta();
				ImGui::DragFloat("Camera Track Delta", &cameraTrackDelta, 0.005f, -0.5f, 10.0f);
				sceneManager.getWritableActiveCamera().setObjectTrackDelta(cameraTrackDelta);
			}
		}

		// Relative camera checkbox
		ImGui::SameLine();
		if (ImGui::Button("Exit Relative Camera"))
		{
			sceneManager.getWritableActiveCamera().unsetRelativeObject();
		}

		entity::GameObjectController* controller = sceneManager.getWritableActiveCamera().getWritableController();
		if (controller != nullptr)
		{
			ImGui::Checkbox("Control Camera", controller->isActivePtr());
		}

		if (ImGui::CollapsingHeader("Camera List"))
		{
			int count = 0;
			auto& cameras = sceneManager.getWritableCameraList();
			for (auto& camera : cameras)
			{
				ImGui::Text(camera->getLabel().c_str());

				ImGui::SameLine();

				if (&(sceneManager.getActiveCamera()) != camera)
				{
					ImGui::PushID(count);
					if (ImGui::Button("Make Active"))
					{
						sceneManager.setActiveCamera(camera);
					}
					ImGui::PopID();
				}
				else
				{
					ImGui::Text(" *Active*");
				}

				++count;
			}
		}

		ImGui::End();
	}

	void GraphicsHandler::update(scene::SceneManager& sceneManager, float deltaTime)
	{
		if (!sceneManager.isPaused())
		{
			float gameTime = sceneManager.getGameTime();
			m_defaultVertexShaderCB.m_data.m_gameTime = gameTime;
			m_waterVertexShaderCB.m_data.m_gameTime = gameTime;
			m_cloudsPixelShaderCB.m_data.m_gameTime = gameTime;
			m_waterPixelShaderCB.m_data.m_gameTime = gameTime;
		}

		// Update atmosphere stuff
		float dayOrNight = 0.0f;
		float zeroToOneDayOrNight = modf(sceneManager.getDayProgress() * 2.0f, &dayOrNight);
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
			sceneManager.getWritableDirectionalLight().setColour(sunColour);
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
			sceneManager.getWritableDirectionalLight().setColour(sunColour);

			m_atmosphericPixelShaderCB.m_data.m_density = 0.25f + density * (0.65f - 0.25f);
		}

		XMVECTOR sunDirection = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationAxis(XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), sceneManager.getDayProgress() * sc_2PI));
		sceneManager.getWritableDirectionalLight().getWritableTransform().setPosition(sunDirection);
		sceneManager.getWritableDirectionalLight().getWritableTransform().lookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

		// Update ImGui
		updateImGui(sceneManager);
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

			swapChainDescription.BufferDesc.Width = UserConfig::it().getWindowWidth();
			swapChainDescription.BufferDesc.Height = UserConfig::it().getWindowHeight();
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
			CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, UserConfig::it().getWindowWidth(), UserConfig::it().getWindowHeight());
			depthStencilTextureDesc.MipLevels = 1;
			depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			hr = m_device->CreateTexture2D(&depthStencilTextureDesc, NULL, m_depthStencilBuffer.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

			hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), NULL, m_depthStencilView.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

			// SET THE RENDER TARGET
			m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

			// CREATE DEPTH STENCIL STATE
			CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

			hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

			// Create viewport
			m_defaultViewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(UserConfig::it().getWindowWidth()), static_cast<float>(UserConfig::it().getWindowHeight()));

			// Create default rasterizer state
			D3D11_RASTERIZER_DESC regularRasterizerDesc;
			ZeroMemory(&regularRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

			regularRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			regularRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

			hr = m_device->CreateRasterizerState(&regularRasterizerDesc, m_regularRasterizerState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create default rasterizer state.");

			// Create wireframe rasterizer state
			D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
			ZeroMemory(&wireFrameRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

			wireFrameRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
			wireFrameRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

			hr = m_device->CreateRasterizerState(&wireFrameRasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

			COM_ERROR_IF_FAILED(hr, "Failed to create wireframe rasterizer state.");

			// Create default blend state
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

			// Create sprite batch and sprite font instances
			m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_deviceContext.Get());
			m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"res\\fonts\\consolas16.spritefont");//comicSansMS16.spritefont

			// Create sampler state
			CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_MIP_LINEAR
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

	void GraphicsHandler::drawAxisForObject(const entity::RenderableGameObject& gameObject, const XMMATRIX& viewProjection, const scene::SceneManager& sceneManager)
	{
		XMFLOAT3 gameObjectPosition = gameObject.getTransform().getPositionFloat3();
		XMMATRIX translationMatrix = XMMatrixTranslation(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);

		float scale = gameObject.getModel().getHitRadius();

		// Clamp scale
		if (scale < 0.75f) scale = 0.75f;

		float distance = XMVectorGetX(XMVector3Length(gameObject.getTransform().getPositionVector() - sceneManager.getActiveCamera().getTransform().getPositionVector()));
		scale *= distance * 0.5f;

		if (sceneManager.getAxisEditState() == scene::AxisEditState::eEditTranslate)
		{
			m_axisTranslateModel->draw(XMMatrixScaling(scale, scale, scale) * translationMatrix, viewProjection, &m_defaultVertexShaderCB);
		}
		else if (sceneManager.getAxisEditState() == scene::AxisEditState::eEditRotate)
		{
			// Multiply by rotation matrix when rotating
			m_axisRotateModel->draw(XMMatrixScaling(scale * 0.75f, scale * 0.75f, scale * 0.75f) * gameObject.getTransform().getRotationMatrix() * translationMatrix, viewProjection, &m_defaultVertexShaderCB);
		}
	}
}