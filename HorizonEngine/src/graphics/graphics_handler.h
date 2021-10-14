
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

// Entities
#include "../entities/camera_game_object.h"
#include "../entities/utility/game_object_controller.h"
#include "../entities/renderable_game_object.h"

// Lights
#include "../entities/lights/light_game_object.h"
#include "../entities/lights/spot_light_game_object.h"
#include "../entities/lights/point_light_game_object.h"

// Physics
#include "../entities/physics/physics_game_object.h"
#include "../entities/physics/particle_system/particle_system.h"
#include "../entities/physics/spring.h"

// Misc
#include "../utils/adapter_reader.h"
#include "../utils/timer.h"
#include "buffers/constant_buffer_types.h"
#include "shaders/shader.h"

//comicSansMS16.spritefont

namespace hrzn::gfx
{
	static const float sc_PI = 3.1415926f;
	static const float sc_2PI = 6.2831853f;

	enum class AxisEditState
	{
		eEditNone,
		eEditTranslate,
		eEditRotate,
		eEditScale,
	};


	enum class AxisEditSubState
	{
		eEditNone,
		eEditX,
		eEditY,
		eEditZ,
	};

	class GraphicsHandler
	{
	public:
		bool initialize(HWND hwnd, int width, int height, entity::ControllerManager* controllerManager);
		void renderFrame(float deltaTime);
		void update(float deltaTime);
		void checkObjectCollisions(float deltaTime);

		void adjustMouseX(int xPos);
		void adjustMouseY(int yPos);
		void setMouseX(int xPos);
		void setMouseY(int yPos);
		void computeMouseNDC();

		void checkSelectingObject();
		void updateSelectedObject();

		XMFLOAT3 readFloat3(std::ifstream& stream);
		XMFLOAT4 readFloat4(std::ifstream& stream);

		void writeFloat3(const XMFLOAT3& float3, std::ofstream& stream);
		void writeFloat3(const XMVECTOR& float3, std::ofstream& stream);
		void writeFloat4(const XMFLOAT4& float4, std::ofstream& stream);
		void writeFloat4(const XMVECTOR& float4, std::ofstream& stream);

		void create3DNoiseTexture();

		bool loadScene(const char* sceneName);
		bool saveScene();
		bool saveSceneTGP();
		void unloadScene();

		void removeGameObject(const std::string& gameObjectLabel);

		void updateImGui();

		entity::CameraGameObject& getCamera();

		entity::GameObject* getGameObject(const std::string& label);

		void                drawAxisForObject(entity::GameObject* gameObject, const XMMATRIX& viewProjection);

		AxisEditSubState    getAxisEditSubState();
		void                stopAxisEdit();

		static XMVECTOR     rayPlaneIntersect(XMVECTOR rayPoint, XMVECTOR rayDirection, XMVECTOR planeNormal, XMVECTOR planePoint);

	private:
		bool  initializeDirectX(HWND hwnd);
		bool  initializeShaders();
		bool  initializeScene();
		void  floatObject(entity::GameObject* object);
		float getWaterHeightAt(float posX, float posZ, bool exact = false);

	private:
		entity::CameraGameObject     m_camera;
		entity::RenderableGameObject m_skybox;
		entity::RenderableGameObject m_clouds;
		entity::RenderableGameObject m_ocean;
		entity::LightGameObject      m_directionalLight;

		std::unordered_map<std::string, entity::GameObject*> m_gameObjectMap;
		std::vector<entity::RenderableGameObject*>           m_renderableGameObjects;
		std::vector<entity::physics::PhysicsGameObject*>     m_physicsGameObjects;
		std::vector<entity::PointLightGameObject*>           m_pointLights;
		std::vector<entity::SpotLightGameObject*>            m_spotLights;

		std::vector<entity::physics::Spring*> m_springs;

		Model m_axisTranslateModel;
		Model m_axisRotateModel;
		Model m_particleMesh;
		Model m_springModel;

		XMFLOAT3 m_xAxisTranslateDefaultBounds;
		XMFLOAT3 m_yAxisTranslateDefaultBounds;
		XMFLOAT3 m_zAxisTranslateDefaultBounds;

		BoundingBox m_xAxisTranslateBoudingBox;
		BoundingBox m_yAxisTranslateBoudingBox;
		BoundingBox m_zAxisTranslateBoudingBox;

		entity::ControllerManager* m_controllerManager;

		entity::physics::ParticleSystem* m_particleSystem;

		gfx::Texture* m_defaultDiffuseTexture;
		gfx::Texture* m_defaultSpecularTexture;
		gfx::Texture* m_defaultNormalTexture;

		gfx::Texture* m_highlightDiffuseTexture;
		gfx::Texture* m_highlightSpecularTexture;
		gfx::Texture* m_highlightNormalTexture;

		Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_deviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		VertexShader m_vertexShader;
		VertexShader m_waterVertexShader;

		PixelShader  m_pixelShader;
		PixelShader  m_noLightPixelShader;
		PixelShader  m_atmosphericPixelShader;
		PixelShader  m_cloudsPixelShader;
		PixelShader  m_waterPixelShader;

		ComputeShader  m_noiseTextureComputeShader;

		ConstantBuffer<VertexShaderCB>              m_vertexShaderCB;
		ConstantBuffer<PixelShaderCB>               m_pixelShaderCB;

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

		std::unordered_map<std::string, entity::GameObjectTrack*> m_objectTracks;

		bool m_selectingGameObject = false;

		entity::RenderableGameObject* m_selectedObject;

		AxisEditState    m_axisEditState = AxisEditState::eEditTranslate;
		AxisEditSubState m_axisEditSubState = AxisEditSubState::eEditNone;

		float    m_lastAxisGrabOffset = FLT_MAX;
		XMVECTOR m_lastGrabPos;

		int m_windowWidth = 0;
		int m_windowHeight = 0;

		int m_mousePosX = 0;
		int m_mousePosY = 0;

		float m_mouseNDCX = 0.0f;
		float m_mouseNDCY = 0.0f;

		utils::Timer m_fpsTimer;
		int          m_fpsCounter;
		std::string  m_fpsString;

		bool m_useWireframe = false;
		bool m_useVSync = true;

		bool m_dayNightCycle = true;
		float m_dayProgress = 0.0f;
		float m_gameTime = 0.0f;
		XMFLOAT3 m_sunDirection;

		bool        m_sceneLoaded = false;
		std::string m_sceneLoadedName;

		int               m_newObjectType;
		bool              m_newObjectMenuOpen = false;
		std::string       m_newObjectLabel = "";
		std::string       m_newObjectModelPath = "";
		const std::string m_defaultObjectModelFilepath = "res/models/";
	};
}
