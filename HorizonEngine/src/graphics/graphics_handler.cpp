
//Function implementations for the Graphics class

#include "graphics_handler.h"

#include <random>
#include <shobjidl.h>

#include "data/resource_manager.h"
#include "../scene/scene_manager.h"
#include "../scene/terrain/terrain_manager.h"

#include "post_process/bloom_post_process.h"
#include "post_process/grayscale_post_process.h"
#include "post_process/sepia_post_process.h"
#include "post_process/depth_of_field_post_process.h"
#include "post_process/edge_detection_post_process.h"
#include "post_process/chromatic_aberration_post_process.h"

#include "../user_config.h"

#define _SOLUTIONDIR R"($(SolutionDir))"

namespace hrzn::gfx
{
	GraphicsHandler& GraphicsHandler::it()
	{
		static GraphicsHandler instance;
		return instance;
	}

	GraphicsHandler::GraphicsHandler() :
		m_device(nullptr),
		m_deviceContext(nullptr),
		
		m_swapChain(nullptr),
		m_swapChainRenderTargetView(nullptr),
		m_backBuffer(nullptr),
		
		m_depthStencilView(nullptr),
		m_depthStencilBuffer(nullptr),
		m_depthStencilState(nullptr),
		
		m_defaultViewport(),
		
		m_activeCameraImageRenderer(),
		
		m_axisTranslateModel(nullptr),
		m_axisRotateModel(nullptr),
		m_springModel(nullptr),
		m_screenQuad(nullptr),
		
		m_compiledShaderFolder(),
		m_defaultVSLayout(nullptr),
		m_defaultVSLayoutSize(0),
		
		m_ps_gbuf_r_default(nullptr),
		
		m_cs_noiseGen(nullptr),
		
		m_noiseTextureCB(),
		m_sceneCB(),
		m_cloudsCB(),
		m_waterCB(),
		m_atmosphericCB(),
		m_perFrameCB(),
		m_perPassCB(),
		m_perMaterialCB(),
		m_perObjectCB(),
		
		m_noiseTexture(nullptr),
		m_noiseTextureUnorderedAccessView(nullptr),
		m_noiseTextureShaderResourceView(nullptr),
		
		m_regularRasterizerState(nullptr),
		m_wireframeRasterizerState(nullptr),

		m_defaultBlendState(nullptr),
		m_gBufferBlendState(nullptr),

		m_samplerState(nullptr),

		m_spriteBatch(nullptr),
		m_spriteFont(nullptr),

		m_useWireframe(false),
		m_useVSync(true),
		m_useDeferredShading(true),
		m_useSSAO(true),

		m_fpsTimer(),
		m_fpsCounter(0),
		m_fpsString()
	{
	}

	bool GraphicsHandler::initialize(HWND hwnd)
	{
		m_fpsTimer.start();

		if (!initializeDirectX(hwnd))
		{
			return false;
		}

		m_compiledShaderFolder = L"";
#pragma region DetermineShaderPath
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		//shaderFolder = L"../x64/Debug/";
		m_compiledShaderFolder = L"../x64/Debug/res/shader/compiled/";
#else  //x86 (Win32)
		m_compiledShaderFolder = L"../Debug/";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		//shaderFolder = L"../x64/Release/";
		m_compiledShaderFolder = L"res/shader/compiled/";
#else  //x86 (Win32)
		m_compiledShaderFolder = L"../Release/";
#endif
#endif

		/***********************************************
		
		MARKING SCHEME: Normal Mapping,	Basic Parallax Mapping and Parallax Occlusion Mapping with self shadowing

		DESCRIPTION: Compatible input layout. All models drawn have normal, tangent and bitangent info calculated
		so that tangent space transform matricies can be computed in the pixel shader (this happens in the pixel shader
		because it is easier to do the lighting calculations in world space, rather than transform all of the light data
		into tangent space)

		COMMENT INDEX: 0

		***********************************************/


		//Create vertex shader input layout
		m_defaultVSLayout = new D3D11_INPUT_ELEMENT_DESC[5]{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		m_defaultVSLayoutSize = 5;

		if (!ResourceManager::it().initialise())
		{
			return false;
		}

		// Initialise gbuffer write shader
		m_ps_gbuf_r_default = ResourceManager::it().getGBufferReadPSPtr();

		// Initialise compute shaders
		m_cs_noiseGen = ResourceManager::it().getCSPtr("noise_gen");

		// Load engine models
		m_axisTranslateModel = ResourceManager::it().getModelPtr("res/models/engine/axis/translate.obj");
		m_axisRotateModel = ResourceManager::it().getModelPtr("res/models/engine/axis/rotate.obj");

		m_springModel = ResourceManager::it().getModelPtr("res/models/engine/spring.obj");

		m_screenQuad = ResourceManager::it().getModelPtr("res/models/engine/screen_quad.obj");

		// Initialise global shader vars
		HRESULT hr = m_noiseTextureCB.initialize(m_device.Get(), m_deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create 'noiseTextureComputeShader' constant buffer.");
		create3DNoiseTexture();

		m_activeCameraImageRenderer.initialise(m_defaultViewport, m_depthStencilState.Get(), m_regularRasterizerState.Get());

		m_activeCameraImageRenderer.addPostProcess(new BloomPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, m_activeCameraImageRenderer.getGBuffer()));
		m_activeCameraImageRenderer.addPostProcess(new DepthOfFieldPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, m_activeCameraImageRenderer.getGBuffer()));
		m_activeCameraImageRenderer.addPostProcess(new ChromaticAberrationPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height));

		// Init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());
		ImGui::StyleColorsDark();

		return true;
	}

	bool GraphicsHandler::postSceneManagerInitialise()
	{
		if (!initialiseConstantBufferData())
		{
			return false;
		}

		if (!initialiseSSAOResources())
		{
			return false;
		}

		return true;
	}

	bool GraphicsHandler::initialiseConstantBufferData()
	{
		try
		{
			// Create constant buffers
			HRESULT hr = m_sceneCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'scene' constant buffer.");

			m_sceneCB.m_data.m_useNormalMapping = true;
			m_sceneCB.m_data.m_useParallaxOcclusionMapping = true;
			m_sceneCB.m_data.m_selfShadowing = true;
			m_sceneCB.m_data.m_roughnessMapping = true;
			m_sceneCB.m_data.m_depthScale = 0.035f;
			m_sceneCB.m_data.m_SSAO = m_useSSAO;

			// This slot shouldn't change over the course of the app
			m_deviceContext->PSSetConstantBuffers(0, 1, m_sceneCB.getAddressOf());

			hr = m_waterCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'waterVertexShader' constant buffer.");

			const scene::config::OceanConfig& oceanConfig = scene::SceneManager::it().getSceneConfig().getOceanConfig();

			m_waterCB.m_data.m_waveCount = oceanConfig.m_waveCount;
			m_waterCB.m_data.m_waveScale = oceanConfig.m_waveScale;   //14.3f
			m_waterCB.m_data.m_wavePeriod = oceanConfig.m_wavePeriod; //50.5f
			m_waterCB.m_data.m_waveSpeed = oceanConfig.m_waveSpeed;   //25.0f
			m_waterCB.m_data.m_waveSeed = oceanConfig.m_waveSeed;
			m_waterCB.m_data.m_waveScaleMultiplier = oceanConfig.m_waveScaleMultiplier;
			m_waterCB.m_data.m_iscolateWaveNum = -1;
			m_waterCB.m_data.m_foamStart = oceanConfig.m_foamStart;
			m_waterCB.m_data.m_colourChangeStart = oceanConfig.m_colourChangeStart; //1.123f

			hr = m_atmosphericCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'atmospheric' constant buffer.");

			const scene::config::AtmosphereConfig& atmosphereConfig = scene::SceneManager::it().getSceneConfig().getAtmosphereConfig();

			m_atmosphericCB.m_data.m_sunSize = atmosphereConfig.m_sunSize;
			m_atmosphericCB.m_data.m_density = atmosphereConfig.m_density;
			m_atmosphericCB.m_data.m_multiScatterPhase = atmosphereConfig.m_multiScatterPhase;
			m_atmosphericCB.m_data.m_anisotropicIntensity = atmosphereConfig.m_anisotropicIntensity;
			m_atmosphericCB.m_data.m_zenithOffset = atmosphereConfig.m_zenithOffset;
			m_atmosphericCB.m_data.m_nightDensity = atmosphereConfig.m_nightDensity;
			m_atmosphericCB.m_data.m_nightZenithYClamp = atmosphereConfig.m_nightZenithYClamp;
			m_atmosphericCB.mapToGPU();

			hr = m_cloudsCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'clouds' constant buffer.");

			const scene::config::CloudConfig& cloudConfig = scene::SceneManager::it().getSceneConfig().getCloudConfig(); // Fluffy 1 // Fluffy 2 // Bulky bois
			
			m_cloudsCB.m_data.m_lightAbsorbtionThroughClouds = cloudConfig.m_lightAbsorbtionThroughClouds; //                0.084f      0.084f      0.338f
			m_cloudsCB.m_data.m_lightAbsorbtionTowardsSun = cloudConfig.m_lightAbsorbtionTowardsSun; //                      0.273f      0.392f      0.559f
			m_cloudsCB.m_data.m_phaseFactor = cloudConfig.m_phaseFactor; //                                                  0.208f      0.266f      0.428f
			m_cloudsCB.m_data.m_darknessThreshold = cloudConfig.m_darknessThreshold; //                                      0.09f       0.073f      0.09f
			m_cloudsCB.m_data.m_cloudCoverage = cloudConfig.m_cloudCoverage; //                                              0.465f      0.497f      0.446f
			m_cloudsCB.m_data.m_cloudSpeed = cloudConfig.m_cloudSpeed;
			m_cloudsCB.m_data.m_numSteps = cloudConfig.m_numSteps;
			m_cloudsCB.m_data.m_stepSize = cloudConfig.m_stepSize;
			m_cloudsCB.m_data.m_cloudHeight = cloudConfig.m_cloudHeight;

			hr = m_ambientOcclusionCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'ambient occlusion' constant buffer.");

			hr = m_perFrameCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'per frame' constant buffer.");

			hr = m_perPassCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'per pass' constant buffer.");

			hr = m_perMaterialCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'per material' constant buffer.");

			hr = m_perObjectCB.initialize(m_device.Get(), m_deviceContext.Get());
			COM_ERROR_IF_FAILED(hr, "Failed to create 'per object' constant buffer.");
		}
		catch (utils::COMException& exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}

		return true;
	}

	bool GraphicsHandler::initialiseSSAOResources()
	{
		m_ambientOcclusionCB.m_data.m_bias = 0.0f;
		m_ambientOcclusionCB.m_data.m_radius = 0.6f;
		m_ambientOcclusionCB.m_data.m_kernelSize = 48;
		m_ambientOcclusionCB.m_data.m_noiseScale = XMFLOAT2(UserConfig::it().getWindowWidthFloat() * 0.25f, UserConfig::it().getWindowHeightFloat() * 0.25f);

		// Create random samples
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		for (int i = 0; i < m_ambientOcclusionCB.m_data.m_kernelSize; ++i)
		{
			XMVECTOR sample = XMVectorSet(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator), 1.0f);
			sample = XMVector3Normalize(sample);
			sample *= randomFloats(generator);
			float scale = static_cast<float>(i) / static_cast<float>(m_ambientOcclusionCB.m_data.m_kernelSize);

			scale = std::lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;

			m_ambientOcclusionCB.m_data.m_randomSamples[i] = XMFLOAT4(XMVectorGetX(sample), XMVectorGetY(sample), XMVectorGetZ(sample), 0.0f);
		}

		m_ambientOcclusionCB.mapToGPU();

		// Noise texture
		std::vector<XMFLOAT4> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			XMFLOAT4 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f, 0.0f);
			ssaoNoise.push_back(noise);
		}

		D3D11_TEXTURE2D_DESC noiseTextureDesc{};
		noiseTextureDesc.Width = 4;
		noiseTextureDesc.Height = 4;
		noiseTextureDesc.MipLevels = 1;
		noiseTextureDesc.ArraySize = 1;
		noiseTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		noiseTextureDesc.SampleDesc.Count = 1;
		noiseTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		noiseTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA ssaoNoiseData;
		ZeroMemory(&ssaoNoiseData, sizeof(ssaoNoiseData));
		ssaoNoiseData.pSysMem = (void*)(&ssaoNoise[0]);
		ssaoNoiseData.SysMemPitch = sizeof(XMFLOAT4);
		ssaoNoiseData.SysMemSlicePitch = sizeof(XMFLOAT4) * 4;

		HRESULT hr = m_device->CreateTexture2D(&noiseTextureDesc, &ssaoNoiseData, m_ambientOcclusionNoiseTexture.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "SSAO noise texture creation failed");

		D3D11_SHADER_RESOURCE_VIEW_DESC noiseTextureShaderResourceViewDesc{};
		noiseTextureShaderResourceViewDesc.Format = noiseTextureDesc.Format;
		noiseTextureShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		noiseTextureShaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = m_device->CreateShaderResourceView(m_ambientOcclusionNoiseTexture.Get(), &noiseTextureShaderResourceViewDesc, m_ambientOcclusionNoiseShaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "SSAO noise texture shader resource view creation failed");

		return true;
	}

	ID3D11Device* GraphicsHandler::getDevice() const
	{
		return m_device.Get();
	}

	ID3D11DeviceContext* GraphicsHandler::getDeviceContext() const
	{
		return m_deviceContext.Get();
	}

	ID3D11DepthStencilView* GraphicsHandler::getDefaultDepthStencilView() const
	{
		return m_depthStencilView.Get();
	}

	ID3D11DepthStencilState* GraphicsHandler::getDefaultDepthStencilState() const
	{
		return m_depthStencilState.Get();
	}

	bool GraphicsHandler::isUsingDeferredShading() const
	{
		return m_useDeferredShading;
	}

	bool GraphicsHandler::isUsingSSAO() const
	{
		return m_useSSAO;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GraphicsHandler::getAmbientOcclusionNoiseShaderResourceView()
	{
		return m_ambientOcclusionNoiseShaderResourceView;
	}

	const std::wstring& GraphicsHandler::getCompiledShaderFolder() const
	{
		return m_compiledShaderFolder;
	}

	D3D11_INPUT_ELEMENT_DESC* GraphicsHandler::getDefaultVSLayout() const
	{
		return m_defaultVSLayout;
	}

	UINT GraphicsHandler::getDefaultVSLayoutSize() const
	{
		return m_defaultVSLayoutSize;
	}

	void GraphicsHandler::render()
	{
		scene::SceneManager::it().getWritableActiveCamera().updateView();

		// Grab active camera data
		auto& activeCamera = scene::SceneManager::it().getActiveCamera();
		XMMATRIX viewMatrix = activeCamera.getViewMatrix();
		XMMATRIX projectionMatrix = activeCamera.getProjectionMatrix();

		XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix;

		DirectX::XMVECTOR activeCamPos = activeCamera.getTransform().getPositionVector();
		DirectX::XMVECTOR activeCamFacing = activeCamera.getTransform().getFrontVector();

		// Render scene from active camera
		m_activeCameraImageRenderer.render(activeCamPos, activeCamFacing, viewMatrix, projectionMatrix, activeCamera.getNearPlane(), activeCamera.getFarPlane());

		// Copy final image to the back buffer
		m_deviceContext->CopyResource(m_backBuffer.Get(), m_activeCameraImageRenderer.getFinalImage().m_texture2D.Get());

		// Set the swap chain as the current render target for UI
		m_deviceContext->OMSetRenderTargets(1, m_swapChainRenderTargetView.GetAddressOf(), m_depthStencilView.Get());

		// Update FPS timer
		m_fpsCounter++;

		if (m_fpsTimer.getMicrosecondsElapsed() > 1000000)
		{
			m_fpsString = "FPS: " + std::to_string(m_fpsCounter);
			m_fpsCounter = 0;
			m_fpsTimer.restart();
		}

		ImGui::Render();

		// Render imgui
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Render UI (not ImGui)
		m_spriteBatch->Begin();

		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(1.0f, 0.5f), DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(m_fpsString).c_str(), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));

		if (scene::SceneManager::it().objectIsSelected())
		{
			// Stop text being drawn behind camera
			if (XMVectorGetX(XMVector3Dot(scene::SceneManager::it().getSelectedObject()->getTransform().getPositionVector() - scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector(), scene::SceneManager::it().getActiveCamera().getTransform().getFrontVector())) >= 0.0f)
			{
				XMFLOAT2 screenNDC = scene::SceneManager::it().getActiveCamera().getNDCFrom3DPos(scene::SceneManager::it().getSelectedObject()->getTransform().getPositionVector() + XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f)); // XMFLOAT2(-0.5,0.5);
				XMFLOAT2 screenPos = DirectX::XMFLOAT2((screenNDC.x * 0.5f + 0.5f) * UserConfig::it().getWindowWidth(), (1.0f - (screenNDC.y * 0.5f + 0.5f)) * UserConfig::it().getWindowHeight());
				XMVECTOR size = m_spriteFont->MeasureString(utils::string_helpers::stringToWide(scene::SceneManager::it().getSelectedObject()->getLabel()).c_str());
				
				screenPos.x -= XMVectorGetX(size) * 0.5f;
				screenPos.y -= XMVectorGetY(size) * 0.5f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(scene::SceneManager::it().getSelectedObject()->getLabel()).c_str(), screenPos, DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
				
				screenPos.y -= 0.5f;
				screenPos.x -= 1.0f;
				m_spriteFont->DrawString(m_spriteBatch.get(), utils::string_helpers::stringToWide(scene::SceneManager::it().getSelectedObject()->getLabel()).c_str(), screenPos, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			}
		}

		m_spriteBatch->End();

		// Present image
		m_swapChain->Present(m_useVSync, NULL);// using vsync
	}

	void GraphicsHandler::renderSkybox(const DirectX::XMVECTOR& eyePos)
	{
		m_atmosphericCB.mapToGPU();
		m_deviceContext->PSSetConstantBuffers(1, 1, m_atmosphericCB.getAddressOf());

		scene::SceneManager::it().getWritableSkybox().getWritableTransform().setPosition(eyePos);
		scene::SceneManager::it().getSkybox().draw(&m_perObjectCB);
	}

	void GraphicsHandler::renderSceneObjects(RenderPassType renderPassType, const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing) // const RenderPassConfig& renderPassConfig
	{
		XMFLOAT3 eyePosFloat = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMStoreFloat3(&eyePosFloat, eyePos);

		bool bindPSData = true;
		if (renderPassType == RenderPassType::eShadowPass)
		{
			bindPSData = false;
		}

		// Terrain
		if (renderPassType == RenderPassType::eNonGBufferCompatiblePass || renderPassType == RenderPassType::eStandardPass)
		{
			scene::TerrainManager::it().renderTerrain(false, bindPSData);
		}

		// Regular objects
		if (renderPassType == RenderPassType::eGBufferCompatiblePass)
		{
			const auto& gBufferRenderables = scene::SceneManager::it().getGBufferRenderables();
			for (size_t i = 0; i < gBufferRenderables.size(); ++i)
			{
				gBufferRenderables[i]->draw(&m_perObjectCB, bindPSData);
			}
		}
		else if (renderPassType == RenderPassType::eNonGBufferCompatiblePass)
		{
			const auto& nonGBufferRenderables = scene::SceneManager::it().getNonGBufferRenderables();
			for (size_t i = 0; i < nonGBufferRenderables.size(); ++i)
			{
				entity::RenderableGameObject* renderable = nonGBufferRenderables[i];

				if (auto* light = dynamic_cast<entity::LightGameObject*>(renderable))
				{
					XMFLOAT3 lightCol = light->getColour();
					m_perMaterialCB.m_data.m_colour = XMFLOAT4(lightCol.x, lightCol.y, lightCol.z, 1.0f);
					m_perMaterialCB.mapToGPU();
				}
				renderable->draw(&m_perObjectCB, bindPSData);
			}
		}
		else if (renderPassType == RenderPassType::eShadowPass)
		{
			const auto& allRenderables = scene::SceneManager::it().getAllRenderables();
			for (size_t i = 0; i < allRenderables.size(); ++i)
			{
				allRenderables[i]->draw(&m_perObjectCB, bindPSData);
			}
		}
		else if (renderPassType == RenderPassType::eStandardPass)
		{
			const auto& allRenderables = scene::SceneManager::it().getAllRenderables();
			for (size_t i = 0; i < allRenderables.size(); ++i)
			{
				entity::RenderableGameObject* renderable = allRenderables[i];
				if (auto* light = dynamic_cast<entity::LightGameObject*>(renderable))
				{
					XMFLOAT3 lightCol = light->getColour();
					m_perMaterialCB.m_data.m_colour = XMFLOAT4(lightCol.x, lightCol.y, lightCol.z, 1.0f);
					m_perMaterialCB.mapToGPU();
				}
				renderable->draw(&m_perObjectCB, bindPSData);
			}
		}
		
		// Draw Springs
		if (renderPassType == RenderPassType::eGBufferCompatiblePass || renderPassType == RenderPassType::eStandardPass)
		{
			XMMATRIX springModelMatrix;

			const auto& springs = scene::SceneManager::it().getSprings();
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

				m_springModel->draw(springModelMatrix, &m_perObjectCB, bindPSData);
			}
		}

		// Draw ocean
		if (renderPassType == RenderPassType::eNonGBufferCompatiblePass || renderPassType == RenderPassType::eStandardPass)
		{
			if (scene::SceneManager::it().getSceneConfig().getOceanConfig().m_enabled)
			{
				m_waterCB.mapToGPU();
				m_deviceContext->VSSetConstantBuffers(1, 1, m_waterCB.getAddressOf());
				m_deviceContext->PSSetConstantBuffers(1, 1, m_waterCB.getAddressOf());

				// Put the centre a bit in front of the camera where the best fidelity is in the mesh
				//float fovDistMod = (1.0f - (fminf(70.0f, scene::SceneManager::it().getActiveCamera().getFOV()) / 70.0f)) * 150.0f;
				XMVECTOR oceanPosition = eyePos + eyeFacing * (abs(eyePosFloat.y) + 30.0f /* + fovDistMod*/) * 1.2f;

				scene::SceneManager::it().getWritableOcean().getWritableTransform().setPosition(XMVectorGetX(oceanPosition), scene::SceneManager::it().getOcean().getTransform().getPositionFloat3().y, XMVectorGetZ(oceanPosition));
				scene::SceneManager::it().getOcean().draw(&m_perObjectCB, bindPSData);
			}
		}

		// Draw clouds
		if (renderPassType == RenderPassType::eNonGBufferCompatiblePass || renderPassType == RenderPassType::eStandardPass)
		{
			if (scene::SceneManager::it().getSceneConfig().getCloudConfig().m_enabled)
			{
				m_cloudsCB.mapToGPU();
				m_deviceContext->PSSetConstantBuffers(1, 1, m_cloudsCB.getAddressOf());
				m_deviceContext->PSSetShaderResources(0, 1, m_noiseTextureShaderResourceView.GetAddressOf());

				scene::SceneManager::it().getWritableClouds().getWritableTransform().setPosition(eyePosFloat.x, scene::SceneManager::it().getClouds().getTransform().getPositionFloat3().y, eyePosFloat.z);
				scene::SceneManager::it().getClouds().draw(&m_perObjectCB, bindPSData);
			}
		}

		// Draw particles
		if (renderPassType == RenderPassType::eNonGBufferCompatiblePass || renderPassType == RenderPassType::eStandardPass)
		{
			scene::SceneManager::it().getParticleSystem().drawParticles(&m_perObjectCB, bindPSData);
		}
	}

	void GraphicsHandler::renderGizmos()
	{
		if (scene::SceneManager::it().objectIsSelected())
		{
			m_perMaterialCB.m_data.m_colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_perMaterialCB.mapToGPU();

			drawAxisForObject(*(scene::SceneManager::it().getSelectedObject()));
		}
	}

	void GraphicsHandler::create3DNoiseTexture()
	{
		int size = 32;
		int height = 32;

		// Set up the constant buffer
		m_deviceContext->CSSetShader(m_cs_noiseGen->getShader(), nullptr, 0);

		m_noiseTextureCB.m_data.m_size = static_cast<float>(size);
		m_noiseTextureCB.m_data.m_height = static_cast<float>(height);
		m_noiseTextureCB.m_data.m_seed = 0.0f;
		m_noiseTextureCB.m_data.m_noiseSize = 1.0f;
		m_noiseTextureCB.mapToGPU();

		m_deviceContext->CSSetConstantBuffers(0, 1, m_noiseTextureCB.getAddressOf());

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

	void GraphicsHandler::updateImGui()
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
		if (scene::SceneManager::it().objectIsSelected())
		{
			entity::GameObject::Type selectedObjectType = scene::SceneManager::it().getSelectedObject()->getType();

			ImGui::Begin("Game Object Settings");

			ImGui::Text(("Label: " + scene::SceneManager::it().getSelectedObject()->getLabel()).c_str());

			ImGui::InputText("Object Label", &scene::SceneManager::it().getWritableSelectedObject()->getLabelPtr()->at(0), 20);

			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::TreePush();

				if (ImGui::CollapsingHeader("Translation"))
				{
					ImGui::TreePush();

					scene::SceneManager::it().getWritableSelectedObject()->getWritableTransform().editPositionImGui();

					if (ImGui::Button("Edit Translation"))
					{
						scene::SceneManager::it().setAxisEditState(scene::AxisEditState::eEditTranslate);
					}

					ImGui::TreePop();
				}

				if (ImGui::CollapsingHeader("Orientation"))
				{
					ImGui::TreePush();

					scene::SceneManager::it().getWritableSelectedObject()->getWritableTransform().displayOrientationImGui();

					if (ImGui::Button("Edit Orientation"))
					{
						scene::SceneManager::it().setAxisEditState(scene::AxisEditState::eEditRotate);
					}

					ImGui::SameLine();

					if (ImGui::Button("Reset Orientation"))
					{
						scene::SceneManager::it().getWritableSelectedObject()->getWritableTransform().setOrientationQuaternion(XMQuaternionIdentity());
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
							scene::SceneManager::it().getWritableSelectedObject()->getWritableTransform().rotateUsingAxis(XMLoadFloat3(&axis), angle);
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

						XMFLOAT3 scale = scene::SceneManager::it().getSelectedObject()->getScale();
						ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.1f, 10.0f);
						scene::SceneManager::it().getWritableSelectedObject()->setScale(scale);

						if (ImGui::Button("Edit Scaling"))
						{
							scene::SceneManager::it().setAxisEditState(scene::AxisEditState::eEditScale);
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

					entity::LightGameObject* lightObj = reinterpret_cast<entity::LightGameObject*>(scene::SceneManager::it().getWritableSelectedObject());
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
						XMVECTOR lightPosition = scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector();
						lightPosition += scene::SceneManager::it().getActiveCamera().getTransform().getFrontVector();
						lightObj->getWritableTransform().setPosition(lightPosition);
						lightObj->getWritableTransform().copyOrientationFrom(scene::SceneManager::it().getActiveCamera().getTransform());
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

					entity::PhysicsGameObject* object = dynamic_cast<entity::PhysicsGameObject*>(scene::SceneManager::it().getWritableSelectedObject());
					ImGui::Checkbox("Static", object->getRigidBody()->isStaticPtr());
					if (ImGui::Button("Apply Upwards Thrust"))
					{
						object->getRigidBody()->addThrust(XMVectorSet(0.0f, 850.0f, 0.0f, 0.0f), 0.5f);
					}
					if (ImGui::Button("Apply Forward Thrust"))
					{
						object->getRigidBody()->addThrust(XMVector3Normalize(object->getTransform().getPositionVector() - scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector()) * 800.0f, 0.5f);
					}
					if (ImGui::Button("Apply Torque"))
					{
						XMVECTOR worldPos = (object->getTransform().getPositionVector() + scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector()) * 0.5f;
						object->getRigidBody()->addTorque(worldPos - object->getTransform().getPositionVector(), XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f));
					}
					if (ImGui::Button("Apply Split Force"))
					{
						XMVECTOR worldPos = scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector();
						object->getRigidBody()->addForceSplit(worldPos, scene::SceneManager::it().getActiveCamera().getTransform().getFrontVector() * 10000.0f);
					}

					ImGui::TreePop();
				}
			}

			if (ImGui::CollapsingHeader("Misc"))
			{
				ImGui::TreePush();

				if (scene::SceneManager::it().getSelectedObject()->hasObjectTrack())
				{
					bool followingTrack = scene::SceneManager::it().getSelectedObject()->isFollowingObjectTrack();
					ImGui::Checkbox("Follow Track", &followingTrack);
					scene::SceneManager::it().getWritableSelectedObject()->setFollowingObjectTrack(followingTrack);
				}

				bool floatingObject = scene::SceneManager::it().getSelectedObject()->getFloating();
				ImGui::Checkbox("Float Object", &floatingObject);
				scene::SceneManager::it().getWritableSelectedObject()->setFloating(floatingObject);

				if (scene::SceneManager::it().getSelectedObject()->isFollowingObject())
				{
					if (ImGui::Button("Stop Following Object"))
					{
						scene::SceneManager::it().getWritableSelectedObject()->setObjectToFollow(nullptr);
					}
				}
				else
				{
					if (ImGui::TreeNode("Followable Objects"))
					{
						auto mapIterator = scene::SceneManager::it().getObjectMap().begin();
						while (mapIterator != scene::SceneManager::it().getObjectMap().end())
						{
							std::string label = "Follow " + mapIterator->second->getLabel();

							if (ImGui::Button(label.c_str()))
							{
								scene::SceneManager::it().getWritableSelectedObject()->setObjectToFollow(mapIterator->second);
							}

							mapIterator++;
						}
						ImGui::TreePop();
					}
				}

				size_t numRelativeCameras = scene::SceneManager::it().getSelectedObject()->getRelativePositions().size();
				if (numRelativeCameras != 0)
				{
					ImGui::Text("Relative Cameras");
				}
				for (size_t i = 0; i < numRelativeCameras; ++i)
				{
					if (ImGui::Button(("Camera " + std::to_string(i + 1)).c_str()))
					{
						scene::SceneManager::it().getWritableActiveCamera().setRelativeObject(scene::SceneManager::it().getSelectedObject(), scene::SceneManager::it().getSelectedObject()->getRelativePositions()[i]);
						scene::SceneManager::it().getWritableActiveCamera().setFollowingObjectTrack(false);
					}
					if (i < numRelativeCameras - 1)
					{
						ImGui::SameLine();
					}
				}

				entity::GameObjectController* controller = scene::SceneManager::it().getWritableSelectedObject()->getWritableController();
				if (controller != nullptr)
				{
					ImGui::Checkbox("Control Object", controller->isActivePtr());
				}

				if (ImGui::Button("Delete Object"))
				{
					scene::SceneManager::it().removeGameObject(scene::SceneManager::it().getSelectedObject()->getLabel());
					scene::SceneManager::it().setSelectedObject(nullptr);
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Close"))
			{
				scene::SceneManager::it().setSelectedObject(nullptr);
			}

			ImGui::End();
		}

		if (ImGui::Begin("Object Selection"))
		{
			auto mapIterator = scene::SceneManager::it().getObjectMap().begin();
			while (mapIterator != scene::SceneManager::it().getObjectMap().end())
			{
				if (entity::RenderableGameObject* gameObject = dynamic_cast<entity::RenderableGameObject*>(mapIterator->second))
				{
					std::string label = gameObject->getLabel();

					if (scene::SceneManager::it().getSelectedObject() == gameObject)
					{
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ("> " + label).c_str());
					}
					else if (ImGui::MenuItem(label.c_str()))
					{
						scene::SceneManager::it().setSelectedObject(gameObject);
					}
				}

				mapIterator++;
			}
		}

		ImGui::End();

		if (ImGui::Begin("Scene Settings"))
		{
			if (ImGui::Button("Save Scene"))
			{
				scene::SceneManager::it().saveScene(scene::SceneManager::it().getSceneName().c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Scene"))
			{
				scene::SceneManager::it().loadScene("test");
			}
			/*ImGui::SameLine();
			if (ImGui::Button("Unload Scene"))
			{
				scene::SceneManager::it().unloadScene();
			}*/

			// Directional light colour
			ImGui::ColorEdit3("Dir Light Colour", &(scene::SceneManager::it().getWritableDirectionalLight().m_colour.x));
			m_sceneCB.m_data.m_directionalLight.m_colour = scene::SceneManager::it().getWritableDirectionalLight().m_colour;

			ImGui::Checkbox("Day/Night Cycle", scene::SceneManager::it().getDayNightCyclePtr());

			ImGui::SameLine();

			ImGui::Checkbox("Pause", scene::SceneManager::it().getPausedPtr());
			ImGui::SliderFloat("Day Progress", scene::SceneManager::it().getDayProgressPtr(), 0.0f, 1.0f);

			if (ImGui::CollapsingHeader("Object Spawning"))
			{
				ImGui::TreePush();

				static char label[20] = "";
				ImGui::InputText("Object Label", &label[0], 30);

				static entity::GameObject::Type chosenType = entity::GameObject::Type::eRenderable;

				if (ImGui::BeginCombo("Object Type", entity::sc_gameObjectTypeToString.at(chosenType).c_str()))
				{
					for (int typeIndex = 1; typeIndex < static_cast<int>(entity::GameObject::Type::eNumTypes); ++typeIndex)
					{
						entity::GameObject::Type type = static_cast<entity::GameObject::Type>(typeIndex);
						bool selected = type == chosenType;
						if (ImGui::Selectable(entity::sc_gameObjectTypeToString.at(type).c_str(), &selected))
						{
							chosenType = type;
						}
					}

					ImGui::EndCombo();
				}

				static std::string path = "res/models/";
				ImGui::InputText("Object Path", &path[0], 256);

				if (chosenType == entity::GameObject::Type::eRenderable)
				{
					if (ImGui::Button("Browse (Must be in 'res/models/')"))
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
										std::wstring widePath = pszFilePath;
										path = std::string(widePath.begin(), widePath.end());
										path = utils::string_helpers::removeBeforeString(path, "res\\models\\");
										CoTaskMemFree(pszFilePath);
									}
									pItem->Release();
								}
							}
							pFileOpen->Release();
						}
					}
				}
				
				if (ImGui::Button("Spawn"))
				{
					std::string labelChosen = label;

					if (labelChosen.empty())
					{
						utils::ErrorLogger::log("Please enter a label");
					}
					else if (labelChosen != "" && scene::SceneManager::it().getObjectMap().find(labelChosen) == scene::SceneManager::it().getObjectMap().end())
					{
						entity::GameObject* gameObject = nullptr;
						switch (chosenType)
						{
						case entity::GameObject::Type::eRenderable:
						{
							entity::RenderableGameObject* newRenderable = new entity::RenderableGameObject();

							newRenderable->initialize(label, path);

							gameObject = newRenderable;

							break;
						}
						case entity::GameObject::Type::ePointLight:
						{
							entity::PointLightGameObject* newPointLight = new entity::PointLightGameObject();

							newPointLight->initialize();
							newPointLight->setLabel(label);

							gameObject = newPointLight;

							break;
						}
						case entity::GameObject::Type::eSpotLight:
						{
							entity::SpotLightGameObject* newSpotLight = new entity::SpotLightGameObject();

							newSpotLight->initialize();
							newSpotLight->setLabel(label);

							gameObject = newSpotLight;

							break;
						}
						}
						
						gameObject->getWritableTransform().setPosition(scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector() + scene::SceneManager::it().getActiveCamera().getTransform().getFrontVector() * 5.0f);

						if (gameObject != nullptr)
						{
							scene::SceneManager::it().addGameObject(gameObject);

							// Select if selectable
							if (auto* renderable = dynamic_cast<entity::RenderableGameObject*>(gameObject))
							{
								scene::SceneManager::it().setSelectedObject(renderable);
							}
						}
					}
					else
					{
						utils::ErrorLogger::log("Label already in use");
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Ocean Options"))
			{
				ImGui::TreePush();

				ImGui::Checkbox("Enabled", &scene::SceneManager::it().getWritableSceneConfig().getWritableOceanConfig().m_enabled);

				ImGui::SliderInt("Wave Count", &m_waterCB.m_data.m_waveCount, 0, 50);
				ImGui::SliderFloat("Wave Scale", &m_waterCB.m_data.m_waveScale, 0.0f, 25.0f);
				ImGui::SliderFloat("Wave Period", &m_waterCB.m_data.m_wavePeriod, 0.0f, 100.0f);
				ImGui::SliderFloat("Wave Speed", &m_waterCB.m_data.m_waveSpeed, 0.0f, 100.0f);
				ImGui::SliderFloat("Wave Seed", &m_waterCB.m_data.m_waveSeed, 100.0f, 1000.0f);
				ImGui::SliderFloat("Wave Scale Multiplier", &m_waterCB.m_data.m_waveScaleMultiplier, 0.0f, 1.0f);
				ImGui::SliderInt("Iscolate Wave Num", &m_waterCB.m_data.m_iscolateWaveNum, -1, 20);
				ImGui::SliderFloat("Foam Start", &m_waterCB.m_data.m_foamStart, 0.0f, 5.0f);
				ImGui::SliderFloat("Colour Change Start", &m_waterCB.m_data.m_colourChangeStart, 0.0f, 5.0f);

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Atmosphere Options"))
			{
				ImGui::TreePush();

				ImGui::SliderFloat("Sun Size", &m_atmosphericCB.m_data.m_sunSize, 10.0f, 200.0f);
				ImGui::SliderFloat("Density", &m_atmosphericCB.m_data.m_density, 0.0f, 3.0f);
				ImGui::SliderFloat("Multi Scatter Phase", &m_atmosphericCB.m_data.m_multiScatterPhase, 0.0f, 3.0f);
				ImGui::SliderFloat("Anisotropic Intensity", &m_atmosphericCB.m_data.m_anisotropicIntensity, -1.0f, 5.0f);
				ImGui::SliderFloat("Zenith Offset", &m_atmosphericCB.m_data.m_zenithOffset, -1.0f, 1.0f);
				ImGui::SliderFloat("Night Density", &m_atmosphericCB.m_data.m_nightDensity, 0.0f, 2.0f);
				ImGui::SliderFloat("Night Zenith Y Clamp", &m_atmosphericCB.m_data.m_nightZenithYClamp, 0.0f, 0.1f);

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Clouds Options"))
			{
				ImGui::TreePush();

				ImGui::Checkbox("Enabled", &scene::SceneManager::it().getWritableSceneConfig().getWritableCloudConfig().m_enabled);

				ImGui::SliderFloat("Absorbtion Through Clouds", &m_cloudsCB.m_data.m_lightAbsorbtionThroughClouds, 0.0f, 2.0f);
				ImGui::SliderFloat("Absorbtion Towards Sun", &m_cloudsCB.m_data.m_lightAbsorbtionTowardsSun, 0.0f, 2.0f);
				ImGui::SliderFloat("Phase Factor", &m_cloudsCB.m_data.m_phaseFactor, 0.0f, 2.0f);
				ImGui::SliderFloat("Darkness Threshold", &m_cloudsCB.m_data.m_darknessThreshold, 0.0f, 1.0f);
				ImGui::SliderFloat("Cloud Coverage", &m_cloudsCB.m_data.m_cloudCoverage, 0.0f, 1.0f);
				ImGui::SliderFloat("Cloud Speed", &m_cloudsCB.m_data.m_cloudSpeed, 0.0f, 0.25f);
				ImGui::SliderFloat("Cloud Height", &m_cloudsCB.m_data.m_cloudHeight, 100.0f, 2000.0f);
				ImGui::SliderInt("Num Steps", &m_cloudsCB.m_data.m_numSteps, 1, 100);
				ImGui::SliderFloat("Step Size", &m_cloudsCB.m_data.m_stepSize, 5.0f, 100.0f);

				ImGui::TreePop();
			}
		}

		ImGui::End();

		if (ImGui::Begin("Render Options"))
		{
			if (ImGui::CollapsingHeader("Post Processing"))
			{
				ImGui::TreePush();

				if (ImGui::Button("Add Bloom (Only for Deferred Shading)"))
				{
					m_activeCameraImageRenderer.addPostProcess(new BloomPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, m_activeCameraImageRenderer.getGBuffer()));
				}

				if (ImGui::Button("Add Depth of Field"))
				{
					m_activeCameraImageRenderer.addPostProcess(new DepthOfFieldPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, m_activeCameraImageRenderer.getGBuffer()));
				}

				ImGui::SameLine();

				if (ImGui::Button("Add Gaussian Blur"))
				{
					m_activeCameraImageRenderer.addPostProcess(new GaussianBlurPostProcess(DXGI_FORMAT_R8G8B8A8_UNORM, (UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, 0.65f));
				}

				if (ImGui::Button("Add Chromatic Abberation"))
				{
					m_activeCameraImageRenderer.addPostProcess(new ChromaticAberrationPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height));
				}

				ImGui::SameLine();

				if (ImGui::Button("Add Grayscale"))
				{
					m_activeCameraImageRenderer.addPostProcess(new GrayscalePostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height));
				}

				if (ImGui::Button("Add Sepia"))
				{
					m_activeCameraImageRenderer.addPostProcess(new SepiaPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height));
				}

				ImGui::SameLine();

				if (ImGui::Button("Add Edge Detection"))
				{
					m_activeCameraImageRenderer.addPostProcess(new EdgeDetectionPostProcess((UINT)m_defaultViewport.Width, (UINT)m_defaultViewport.Height, m_activeCameraImageRenderer.getGBuffer()));
				}

				ImGui::Text("Active Post Processes:");

				auto& postProcesses = m_activeCameraImageRenderer.getPostProcesses();
				for (int postProcessIndex = 0; postProcessIndex < postProcesses.size(); ++postProcessIndex)
				{
					ImGui::Text("%d -> %s", postProcessIndex, postProcesses[postProcessIndex]->getName());
				}

				if (postProcesses.empty())
				{
					ImGui::Text("None Set");
				}
				else
				{
					if (ImGui::Button("Clear Post Processes"))
					{
						m_activeCameraImageRenderer.clearPostProcesses();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Mapping Techniques"))
			{
				ImGui::TreePush();

				/***********************************************

				MARKING SCHEME: Normal Mapping,	Basic Parallax Mapping and Parallax Occlusion Mapping with self shadowing

				DESCRIPTION: This is where the app lets the user select the mapping techniques they would like active.
				These values are written to the correct constant buffer and will be mapped to the gpu later on

				COMMENT INDEX: 1

				***********************************************/

				// Normal Mapping
				bool useNormalMapping = static_cast<bool>(m_sceneCB.m_data.m_useNormalMapping);
				ImGui::Checkbox("Normal Mapping", &useNormalMapping);
				m_sceneCB.m_data.m_useNormalMapping = useNormalMapping;

				ImGui::SameLine();

				// PO Mapping
				bool useParallaxOcclusionMapping = static_cast<bool>(m_sceneCB.m_data.m_useParallaxOcclusionMapping);
				ImGui::Checkbox("PO Mapping", &useParallaxOcclusionMapping);
				m_sceneCB.m_data.m_useParallaxOcclusionMapping = useParallaxOcclusionMapping;

				ImGui::SameLine();

				// Roughness Mapping
				bool roughnessMapping = static_cast<bool>(m_sceneCB.m_data.m_roughnessMapping);
				ImGui::Checkbox("Roughness Mapping", &roughnessMapping);
				m_sceneCB.m_data.m_roughnessMapping = roughnessMapping;

				// Self shadowing
				bool selfShadowing = static_cast<bool>(m_sceneCB.m_data.m_selfShadowing);
				ImGui::Checkbox("Self Shadowing (Not Compatible w/ Deferred Shading)", &selfShadowing);
				m_sceneCB.m_data.m_selfShadowing = selfShadowing;

				// Depth scale
				ImGui::DragFloat("Depth Scale", &m_sceneCB.m_data.m_depthScale, 0.001f, 0.0f, 0.5f);

				ImGui::TreePop();
			}

			if (ImGui::CollapsingHeader("Misc"))
			{
				ImGui::TreePush();

				// Show Normals
				bool showWorldNormals = static_cast<bool>(m_sceneCB.m_data.m_showWorldNormals);
				ImGui::Checkbox("Normals", &showWorldNormals);
				m_sceneCB.m_data.m_showWorldNormals = showWorldNormals;

				ImGui::SameLine();

				// Show UVs
				bool showUVs = static_cast<bool>(m_sceneCB.m_data.m_showUVs);
				ImGui::Checkbox("Show UVs", &showUVs);
				m_sceneCB.m_data.m_showUVs = showUVs;

				ImGui::SameLine();

				// Cull back normals
				bool cullBackNormals = static_cast<bool>(m_sceneCB.m_data.m_cullBackNormals);
				ImGui::Checkbox("Cull Back Normals", &cullBackNormals);
				m_sceneCB.m_data.m_cullBackNormals = cullBackNormals;

				// Gamma correction
				bool gammaCorrection = static_cast<bool>(m_sceneCB.m_data.m_gammaCorrection);
				ImGui::Checkbox("Gamma Corr", &gammaCorrection);
				m_sceneCB.m_data.m_gammaCorrection = gammaCorrection;

				ImGui::SameLine();

				// Misc toggle A
				bool miscToggleA = static_cast<bool>(m_sceneCB.m_data.m_miscToggleA);
				ImGui::Checkbox("Misc A", &miscToggleA);
				m_sceneCB.m_data.m_miscToggleA = miscToggleA;

				ImGui::SameLine();

				// Misc toggle B
				bool miscToggleB = static_cast<bool>(m_sceneCB.m_data.m_miscToggleB);
				ImGui::Checkbox("Misc B", &miscToggleB);
				m_sceneCB.m_data.m_miscToggleB = miscToggleB;

				ImGui::Checkbox("Use VSync", &m_useVSync);

				ImGui::TreePop();
			}

			/***********************************************

			MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

			DESCRIPTION: This is where Deferred shading and Ambient occlusion can be toggled,
			their textures can also be viewed by expanding the "View Textures" collapsing header.

			COMMENT INDEX: 8

			***********************************************/

			if (ImGui::CollapsingHeader("Deferred Shading"))
			{
				ImGui::TreePush();

				ImGui::Checkbox("Deferred Shading", &m_useDeferredShading);

				ImGui::Checkbox("Ambient Occlusion", &m_useSSAO);
				m_sceneCB.m_data.m_SSAO = m_useSSAO;

				if (ImGui::CollapsingHeader("View Textures"))
				{
					ImGui::TreePush();

					ImGui::Text("RGBA -> Albedo                                     RGB -> Position + A -> Roughness");

					ImVec2 textureSize(UserConfig::it().getWindowWidthFloat() * 0.2f, UserConfig::it().getWindowHeightFloat() * 0.2f);
					ImGui::Image(m_activeCameraImageRenderer.getGBuffer()->m_albedo.m_shaderResourceView.Get(), textureSize);

					ImGui::SameLine();

					ImGui::Image(m_activeCameraImageRenderer.getGBuffer()->m_positionRoughness.m_shaderResourceView.Get(), textureSize);

					ImGui::Text("RGB -> Normal + A -> Material AO                   RBG -> Emission + A -> Metallic");

					ImGui::Image(m_activeCameraImageRenderer.getGBuffer()->m_normalAO.m_shaderResourceView.Get(), textureSize);

					ImGui::SameLine();

					ImGui::Image(m_activeCameraImageRenderer.getGBuffer()->m_emissionMetallic.m_shaderResourceView.Get(), textureSize);

					ImGui::Text("R -> Screen Space Ambient Occlusion");

					ImGui::Image(m_activeCameraImageRenderer.getRenderedAmbientOcclusionTexture()->m_shaderResourceView.Get(), textureSize);

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::Button("Toggle Wireframe"))
			{
				m_useWireframe = !m_useWireframe;

				if (m_useWireframe) m_activeCameraImageRenderer.setRasterizerState(m_wireframeRasterizerState.Get());
				else                m_activeCameraImageRenderer.setRasterizerState(m_regularRasterizerState.Get());
			}
		}

		ImGui::End();

		if (ImGui::Begin("Particle System Settings"))
		{
			physics::ParticleSystem& particleSystem = scene::SceneManager::it().getWritableParticleSystem();

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
		}

		ImGui::End();

		if (ImGui::Begin("Camera Settings"))
		{
			scene::SceneManager::it().getWritableActiveCamera().getWritableTransform().editPositionImGui();
			scene::SceneManager::it().getActiveCamera().getTransform().displayOrientationImGui();

			ImGui::NewLine();

			if (scene::SceneManager::it().getActiveCamera().hasObjectTrack())
			{
				// Camera track checkbox
				bool followTrack = scene::SceneManager::it().getActiveCamera().isFollowingObjectTrack();
				ImGui::Checkbox("Camera Follow Track", &followTrack);
				scene::SceneManager::it().getWritableActiveCamera().setFollowingObjectTrack(followTrack);

				if (followTrack)
				{
					// Camera track delta
					float cameraTrackDelta = scene::SceneManager::it().getActiveCamera().getObjectTrackDelta();
					ImGui::DragFloat("Camera Track Delta", &cameraTrackDelta, 0.005f, -0.5f, 10.0f);
					scene::SceneManager::it().getWritableActiveCamera().setObjectTrackDelta(cameraTrackDelta);
				}
			}

			// Relative camera checkbox
			ImGui::SameLine();
			if (ImGui::Button("Exit Relative Camera"))
			{
				scene::SceneManager::it().getWritableActiveCamera().unsetRelativeObject();
			}

			entity::GameObjectController* controller = scene::SceneManager::it().getWritableActiveCamera().getWritableController();
			if (controller != nullptr)
			{
				ImGui::Checkbox("Control Camera", controller->isActivePtr());
			}

			if (ImGui::CollapsingHeader("Camera List", ImGuiTreeNodeFlags_DefaultOpen))
			{
				int count = 0;
				auto& cameras = scene::SceneManager::it().getWritableCameraList();
				for (auto& camera : cameras)
				{
					ImGui::Text(camera->getLabel().c_str());

					ImGui::SameLine();

					if (&(scene::SceneManager::it().getActiveCamera()) != camera)
					{
						ImGui::PushID(count);
						if (ImGui::Button("Make Active"))
						{
							scene::SceneManager::it().setActiveCamera(camera);
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
		}

		ImGui::End();
	}

	ConstantBuffer<SceneCB>& GraphicsHandler::getSceneCB()
	{
		return m_sceneCB;
	}

	ConstantBuffer<CloudsCB>& GraphicsHandler::getCloudsCB()
	{
		return m_cloudsCB;
	}

	ConstantBuffer<WaterCB>& GraphicsHandler::getWaterCB()
	{
		return m_waterCB;
	}

	ConstantBuffer<AtmosphericCB>& GraphicsHandler::getAtmosphericCB()
	{
		return m_atmosphericCB;
	}

	ConstantBuffer<PerFrameCB>& GraphicsHandler::getPerFrameCB()
	{
		return m_perFrameCB;
	}

	ConstantBuffer<PerPassCB>& GraphicsHandler::getPerPassCB()
	{
		return m_perPassCB;
	}

	ConstantBuffer<PerMaterialCB>& GraphicsHandler::getPerMaterialCB()
	{
		return m_perMaterialCB;
	}

	ConstantBuffer<PerObjectCB>& GraphicsHandler::getPerObjectCB()
	{
		return m_perObjectCB;
	}

	ConstantBuffer<AmbientOcclusionCB>& GraphicsHandler::getAmbientOcclusionCB()
	{
		return m_ambientOcclusionCB;
	}

	ID3D11BlendState* GraphicsHandler::getDefaultBlendState()
	{
		return m_defaultBlendState.Get();
	}

	ID3D11BlendState* GraphicsHandler::getGBufferBlendState()
	{
		return m_gBufferBlendState.Get();
	}

	ID3D11RasterizerState* GraphicsHandler::getDefaultRasterizerState()
	{
		return m_regularRasterizerState.Get();
	}

	Model* GraphicsHandler::getScreenQuad() const
	{
		return m_screenQuad;
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState>& GraphicsHandler::getSamplerState()
	{
		return m_samplerState;
	}

	void GraphicsHandler::update(float deltaTime)
	{
		if (!scene::SceneManager::it().isPaused())
		{
			float gameTime = scene::SceneManager::it().getGameTime();
			m_perFrameCB.m_data.m_gameTime = gameTime;
			m_perFrameCB.mapToGPU();
		}

		// Update atmosphere stuff
		float dayOrNight = 0.0f;
		float zeroToOneDayOrNight = modf(scene::SceneManager::it().getDayProgress() * 2.0f, &dayOrNight);
		float split = 1.0f - abs(zeroToOneDayOrNight - 0.5f) * 2.0f;

		if (dayOrNight == 0.0f)
		{
			float t = fmaxf(0.0f, fminf(1.0f, (split - 0.25f) / (0.1f - 0.25f)));
			float densityMod = t * t * (3.0f - 2.0f * t);
			m_atmosphericCB.m_data.m_density = 0.142f + densityMod * (0.65f - 0.142f);

			XMFLOAT3 sunsetColour = XMFLOAT3(1.0f, 0.62f, 0.26f);
			XMFLOAT3 daySunColour = XMFLOAT3(1.0f, 0.85f, 0.65f);

			XMFLOAT3 sunColour = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMStoreFloat3(&sunColour, XMVectorLerp(XMLoadFloat3(&daySunColour), XMLoadFloat3(&sunsetColour), t));
			scene::SceneManager::it().getWritableDirectionalLight().setColour(sunColour);
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
			scene::SceneManager::it().getWritableDirectionalLight().setColour(sunColour);

			m_atmosphericCB.m_data.m_density = 0.25f + density * (0.65f - 0.25f);
		}

		XMVECTOR sunDirection = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationAxis(XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), scene::SceneManager::it().getDayProgress() * sc_2PI));
		scene::SceneManager::it().getWritableDirectionalLight().getWritableTransform().setPosition(sunDirection);
		scene::SceneManager::it().getWritableDirectionalLight().getWritableTransform().lookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

		updateShadowMap();

		// Update ImGui
		updateImGui();
	}

	void GraphicsHandler::updateShadowMap()
	{

	}

	void GraphicsHandler::updateSceneShaderValues()
	{
		// Point light shader variables
		const auto& pointLights = scene::SceneManager::it().getPointLights();
		size_t numPointLights = pointLights.size();
		for (size_t i = 0; i < numPointLights; ++i)
		{
			pointLights[i]->updateShaderVariables(m_sceneCB, i);
		}

		// Spot light shader variables
		const auto& spotLights = scene::SceneManager::it().getSpotLights();
		size_t numSpotLights = spotLights.size();
		for (size_t i = 0; i < numSpotLights; ++i)
		{
			spotLights[i]->updateShaderVariables(m_sceneCB, i);
		}

		scene::SceneManager::it().getWritableDirectionalLight().updateShaderVariables(m_sceneCB);

		// General shader variables
		m_sceneCB.m_data.m_numPointLights = static_cast<int>(numPointLights);
		m_sceneCB.m_data.m_numSpotLights = static_cast<int>(numSpotLights);
		m_sceneCB.mapToGPU();
	}

	void GraphicsHandler::updatePerFrameShaderValues()
	{
		m_perFrameCB.m_data.m_gameTime = scene::SceneManager::it().getGameTime();
		m_perFrameCB.mapToGPU();
	}

	void GraphicsHandler::updatePerPassShaderValues(DirectX::XMFLOAT3 eyePosition, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, float nearPlane, float farPlane)
	{
		m_perPassCB.m_data.m_cameraPosition = eyePosition;

		m_perPassCB.m_data.m_viewProjectionMatrix = view * projection;

		m_perPassCB.m_data.m_nearPlane = nearPlane;
		m_perPassCB.m_data.m_farPlane = farPlane;

		m_perPassCB.mapToGPU();
	}

	bool GraphicsHandler::initializeDirectX(HWND hwnd)
	{
		try
		{
			// Get graphics card adapters
			std::vector<utils::AdapterData> adapters = utils::AdapterReader::getAdapters();

			if (adapters.size() < 1)
			{
				utils::ErrorLogger::log("No DXGI Adapters Found.");
				return false;
			}

			// Create device and swapchain
			DXGI_SWAP_CHAIN_DESC swapChainDescription;
			ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

			swapChainDescription.BufferDesc.Width = UserConfig::it().getWindowWidth();
			swapChainDescription.BufferDesc.Height = UserConfig::it().getWindowHeight();
			swapChainDescription.BufferDesc.RefreshRate.Numerator = 120; // VSync fps
			swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;

			swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swapChainDescription.SampleDesc.Count = 1;   // No anti aliasing
			swapChainDescription.SampleDesc.Quality = 0; // Lowest image quality

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

			// Get backbuffer
			hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBuffer.GetAddressOf()));
			COM_ERROR_IF_FAILED(hr, "Failed to get back buffer.");

			// Create render target view
			hr = m_device->CreateRenderTargetView(m_backBuffer.Get(), NULL, m_swapChainRenderTargetView.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

			// Create depth stencil texture and view
			{
				CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, UserConfig::it().getWindowWidth(), UserConfig::it().getWindowHeight());
				depthStencilTextureDesc.MipLevels = 1;
				depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

				hr = m_device->CreateTexture2D(&depthStencilTextureDesc, NULL, m_depthStencilBuffer.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

				hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), NULL, m_depthStencilView.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
			}

			// Create depth stencil state
			{
				CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

				hr = m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
			}

			// Create viewport
			m_defaultViewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(UserConfig::it().getWindowWidth()), static_cast<float>(UserConfig::it().getWindowHeight()));

			// Create default rasterizer state
			{
				D3D11_RASTERIZER_DESC regularRasterizerDesc;
				ZeroMemory(&regularRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

				regularRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				regularRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

				hr = m_device->CreateRasterizerState(&regularRasterizerDesc, m_regularRasterizerState.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create default rasterizer state.");
			}

			// Create wireframe rasterizer state
			{
				D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
				ZeroMemory(&wireFrameRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

				wireFrameRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
				wireFrameRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

				hr = m_device->CreateRasterizerState(&wireFrameRasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

				COM_ERROR_IF_FAILED(hr, "Failed to create wireframe rasterizer state.");
			}

			// Create default blend state
			{
				D3D11_BLEND_DESC defaultBlendDesc;
				ZeroMemory(&defaultBlendDesc, sizeof(defaultBlendDesc));

				D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc;
				ZeroMemory(&defaultRenderTargetBlendDesc, sizeof(defaultRenderTargetBlendDesc));

				defaultRenderTargetBlendDesc.BlendEnable = true;
				defaultRenderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
				defaultRenderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				defaultRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				defaultRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
				defaultRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
				defaultRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				defaultRenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

				defaultBlendDesc.RenderTarget[0] = defaultRenderTargetBlendDesc;

				hr = m_device->CreateBlendState(&defaultBlendDesc, m_defaultBlendState.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create default blend state.");
			}

			// Create gbuffer blendstate
			{
				D3D11_BLEND_DESC gBufferBlendDesc;
				ZeroMemory(&gBufferBlendDesc, sizeof(gBufferBlendDesc));

				D3D11_RENDER_TARGET_BLEND_DESC gBufferRenderTargetBlendDesc;
				ZeroMemory(&gBufferRenderTargetBlendDesc, sizeof(gBufferRenderTargetBlendDesc));

				gBufferRenderTargetBlendDesc.BlendEnable = true;
				gBufferRenderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
				gBufferRenderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
				gBufferRenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				gBufferRenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
				gBufferRenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
				gBufferRenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				gBufferRenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

				for (int RTVIndex = 0; RTVIndex < 8; ++RTVIndex)
				{
					gBufferBlendDesc.RenderTarget[0] = gBufferRenderTargetBlendDesc;
				}

				hr = m_device->CreateBlendState(&gBufferBlendDesc, m_gBufferBlendState.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create gbuffer blend state.");
			}

			// Create sampler state
			{
				CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_MIP_LINEAR
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

				hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
				COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
			}

			// Create sprite batch and sprite font instances
			m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_deviceContext.Get());
			m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_device.Get(), L"res\\fonts\\consolas16.spritefont");//comicSansMS16.spritefont
		}
		catch (utils::COMException & exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}
		return true;
	}

	void GraphicsHandler::drawAxisForObject(const entity::RenderableGameObject& gameObject)
	{
		XMFLOAT3 gameObjectPosition = gameObject.getTransform().getPositionFloat3();
		XMMATRIX translationMatrix = XMMatrixTranslation(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);

		float scale = gameObject.getModel().getHitRadius();

		// Clamp scale
		if (scale < 0.75f) scale = 0.75f;

		float distance = XMVectorGetX(XMVector3Length(gameObject.getTransform().getPositionVector() - scene::SceneManager::it().getActiveCamera().getTransform().getPositionVector()));
		scale *= distance * 0.5f;

		if (scene::SceneManager::it().getAxisEditState() == scene::AxisEditState::eEditTranslate)
		{
			m_axisTranslateModel->draw(XMMatrixScaling(scale, scale, scale) * translationMatrix, &m_perObjectCB);
		}
		else if (scene::SceneManager::it().getAxisEditState() == scene::AxisEditState::eEditRotate)
		{
			// Multiply by rotation matrix when rotating
			m_axisRotateModel->draw(XMMatrixScaling(scale * 0.75f, scale * 0.75f, scale * 0.75f) * gameObject.getTransform().getRotationMatrix() * translationMatrix, &m_perObjectCB);
		}
	}
}