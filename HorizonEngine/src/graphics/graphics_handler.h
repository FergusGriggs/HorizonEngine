
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

// Misc
#include "../utils/adapter_reader.h"
#include "../utils/timer.h"

#include "buffers/constant_buffer_types.h"
#include "buffers/geometry_buffer.h"

#include "../entity/renderable_game_object.h"

#include "shaders/shader.h"
#include "render_pass_config.h"

#include "image_renderer.h"

// comicSansMS16.spritefont

namespace hrzn::gfx
{
	static const float sc_PI = 3.1415926f;
	static const float sc_2PI = 6.2831853f;

	enum class RenderPassType
	{
		eShadowPass,
		eGBufferCompatiblePass,
		eNonGBufferCompatiblePass,
		eStandardPass,
	};

	class GraphicsHandler
	{
	public:
		static GraphicsHandler& it();

		bool initialize(HWND hwnd);
		bool initializeScene();

		ID3D11Device*        getDevice() const;
		ID3D11DeviceContext* getDeviceContext() const;

		ID3D11DepthStencilView*   getDefaultDepthStencilView() const;
		ID3D11DepthStencilState*  getDefaultDepthStencilState() const;
		
		bool                      isUsingDeferredShading() const;
		const std::wstring&       getCompiledShaderFolder() const;
		D3D11_INPUT_ELEMENT_DESC* getDefaultVSLayout() const;
		UINT                      getDefaultVSLayoutSize() const;

		ConstantBuffer<SceneCB>&       getSceneCB();
		ConstantBuffer<CloudsCB>&      getCloudsCB();
		ConstantBuffer<WaterCB>&       getWaterCB();
		ConstantBuffer<AtmosphericCB>& getAtmosphericCB();
		ConstantBuffer<PerFrameCB>&    getPerFrameCB();
		ConstantBuffer<PerPassCB>&     getPerPassCB();
		ConstantBuffer<PerMaterialCB>& getPerMaterialCB();
		ConstantBuffer<PerObjectCB>&   getPerObjectCB();

		ID3D11BlendState* getDefaultBlendState();
		ID3D11BlendState* getGBufferBlendState();

		Model* getScreenQuad() const;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>& getSamplerState();

		void update(float deltaTime);
		void updateShadowMap();

		void updateSceneShaderValues();
		void updatePerFrameShaderValues();
		void updatePerPassShaderValues(DirectX::XMFLOAT3 eyePosition, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

		void render();

		void renderSkybox(const DirectX::XMVECTOR& eyePos);
		void renderSceneObjects(RenderPassType renderPassType, const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing);
		void renderGizmos();

		void create3DNoiseTexture();

		void updateImGui();

		void drawAxisForObject(const entity::RenderableGameObject& gameObject);

	private:
		GraphicsHandler();

		bool  initializeDirectX(HWND hwnd);

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;

		Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_swapChainRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_backBuffer;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

		CD3D11_VIEWPORT m_defaultViewport;

		ImageRenderer m_activeCameraImageRenderer;
		//ImageRenderer m_cubemapImageRenderer;

		Model* m_axisTranslateModel;
		Model* m_axisRotateModel;
		Model* m_springModel;
		Model* m_screenQuad;

		std::wstring              m_compiledShaderFolder;
		D3D11_INPUT_ELEMENT_DESC* m_defaultVSLayout;
		UINT                      m_defaultVSLayoutSize;

		PixelShader*  m_ps_gbuf_r_default;

		ComputeShader*  m_cs_noiseGen;

		// Constant buffers
		ConstantBuffer<NoiseTextureCB> m_noiseTextureCB;

		// Main pipeline
		// Shader-specific frame-independent CBs || Slot 0 & 1
		// Slot 0
		ConstantBuffer<SceneCB>        m_sceneCB;

		// Slot 1
		ConstantBuffer<CloudsCB>       m_cloudsCB;
		ConstantBuffer<WaterCB>        m_waterCB;
		ConstantBuffer<AtmosphericCB>  m_atmosphericCB;

		// Shader-specific per-frame CBs || Slot 2 & 3
		// Slot 2
		ConstantBuffer<PerFrameCB>     m_perFrameCB;

		// Shader-specific per-pass CBs || Slot 4 & 5
		// Slot 4
		ConstantBuffer<PerPassCB>      m_perPassCB;

		// Shader-specific per-pass CBs || Slot 6 & 7
		// Slot 6
		ConstantBuffer<PerMaterialCB>  m_perMaterialCB;

		// Shader-specific per-object CBs || Slot 8 -> 9
		// Slot 8
		ConstantBuffer<PerObjectCB>    m_perObjectCB;

		// Noise texture vars
		Microsoft::WRL::ComPtr<ID3D11Texture3D>           m_noiseTexture;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_noiseTextureUnorderedAccessView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_noiseTextureShaderResourceView;

		// Default state vars
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_regularRasterizerState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;

		Microsoft::WRL::ComPtr<ID3D11BlendState>      m_defaultBlendState;
		Microsoft::WRL::ComPtr<ID3D11BlendState>      m_gBufferBlendState;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>    m_samplerState;

		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
		std::unique_ptr<DirectX::SpriteFont>  m_spriteFont;

		bool m_useWireframe;
		bool m_useVSync;
		bool m_useDeferredShading;

		utils::Timer m_fpsTimer;
		int          m_fpsCounter;
		std::string  m_fpsString;
	};
}
