
//Handles most DirectX initialization and all object initialization, updating and rendering

#pragma once

#include <unordered_map>
#include <fstream>

#include <WICTextureLoader.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "data/resource_manager.h"
#include "../scene/scene_manager.h"

// Misc
#include "../utils/adapter_reader.h"
#include "../utils/timer.h"
#include "buffers/constant_buffer_types.h"
#include "buffers/geometry_buffer.h"
#include "shaders/shader.h"
#include "render_pass_config.h"

//comicSansMS16.spritefont

namespace hrzn::gfx
{
	static const float sc_PI = 3.1415926f;
	static const float sc_2PI = 6.2831853f;

	class GraphicsHandler
	{
	public:
		bool initialize(HWND hwnd);
		bool initializeScene(scene::SceneManager& sceneManager);

		void update(scene::SceneManager& sceneManager, float deltaTime);

		void render(scene::SceneManager& sceneManager);
		void renderSceneObjects(scene::SceneManager& sceneManager, const RenderPassConfig& renderPassConfig);

		void create3DNoiseTexture();
		void createRenderPassConfigs();

		void updateImGui(scene::SceneManager& sceneManager);

		void drawAxisForObject(const entity::RenderableGameObject& gameObject, const XMMATRIX& viewProjection, const scene::SceneManager& sceneManager);

	private:
		bool  initializeDirectX(HWND hwnd);
		bool  initializeShaders();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;

		Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		CD3D11_VIEWPORT m_defaultViewport;

		Model* m_axisTranslateModel;
		Model* m_axisRotateModel;
		Model* m_springModel;
		Model* m_quadModel;

		gfx::Texture* m_defaultDiffuseTexture;
		gfx::Texture* m_defaultSpecularTexture;
		gfx::Texture* m_defaultNormalTexture;

		gfx::Texture* m_highlightDiffuseTexture;
		gfx::Texture* m_highlightSpecularTexture;
		gfx::Texture* m_highlightNormalTexture;

		VertexShader m_defaultVertexShader;
		VertexShader m_waterVertexShader;
		VertexShader m_quadVertexShader;

		PixelShader  m_lightingPixelShader;
		PixelShader  m_noLightPixelShader;
		PixelShader  m_atmosphericPixelShader;
		PixelShader  m_cloudsPixelShader;
		PixelShader  m_waterPixelShader;
		PixelShader  m_gBufferPixelShader;

		ComputeShader  m_noiseTextureComputeShader;

		ConstantBuffer<VertexShaderCB>              m_defaultVertexShaderCB;
		ConstantBuffer<PixelShaderCB>               m_lightingPixelShaderCB;

		ConstantBuffer<WaterVertexShaderCB>         m_waterVertexShaderCB;
		ConstantBuffer<WaterPixelShaderCB>          m_waterPixelShaderCB;
		ConstantBuffer<NoLightPixelShaderCB>        m_noLightPixelShaderCB;
		ConstantBuffer<AtmosphericPixelShaderCB>    m_atmosphericPixelShaderCB;
		ConstantBuffer<CloudsPixelShaderCB>         m_cloudsPixelShaderCB;

		ConstantBuffer<NoiseTextureComputeShaderCB>       m_noiseTextureComputeShaderCB;
		Microsoft::WRL::ComPtr<ID3D11Texture3D>           m_noiseTexture;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_noiseTextureUnorderedAccessView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_noiseTextureShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_regularRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11BlendState>      m_blendState;

		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
		std::unique_ptr<DirectX::SpriteFont>  m_spriteFont;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

		GeometryBuffer m_geometryBuffer;

		RenderPassConfig m_defaultRenderConfig;
		RenderPassConfig m_shadowPassRenderConfig;

		bool m_useWireframe;
		bool m_useVSync;

		utils::Timer m_fpsTimer;
		int          m_fpsCounter;
		std::string  m_fpsString;
	};
}
