//Graphics.h
//Handles most DirectX initialization and all object initialization, updating and rendering

#pragma once

#include "utility/AdapterReader.h"
#include "Shader.h"
#include "buffers/ConstantBufferTypes.h"
#include "Camera.h"
#include "../utility/Timer.h"
#include "RenderableGameObject.h"
#include "PhysicsGameObject.h"
#include "lights/Light.h"
#include "lights/SpotLight.h"
#include "lights/PointLight.h"
#include "utility/ResourceManager.h"
#include "utility/Controller.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>

#include <WICTextureLoader.h>

#include <unordered_map>
#include <fstream>

//comicSansMS16.spritefont

enum class AxisEditState {
	EDIT_NONE,
	EDIT_TRANSLATE,
	EDIT_ROTATE,
	EDIT_SCALE,
};


enum class AxisEditSubState {
	EDIT_NONE,
	EDIT_X,
	EDIT_Y,
	EDIT_Z,
};

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height, ControllerManager* controllerManager);
	void RenderFrame(float deltaTime);
	void Update(float deltaTime);
	void CheckObjectCollisions();

	void AdjustMouseX(int xPos);
	void AdjustMouseY(int yPos);
	void SetMouseX(int xPos);
	void SetMouseY(int yPos);
	void ComputeMouseNDC();

	void CheckSelectingObject();
	void UpdateSelectedObject();

	XMFLOAT3 ReadFloat3(std::ifstream& stream);
	XMFLOAT4 ReadFloat4(std::ifstream& stream);

	void WriteFloat3(const XMFLOAT3& float3, std::ofstream& stream);
	void WriteFloat3(const XMVECTOR& float3, std::ofstream& stream);
	void WriteFloat4(const XMFLOAT4& float4, std::ofstream& stream);
	void WriteFloat4(const XMVECTOR& float4, std::ofstream& stream);

	bool LoadScene(const char* sceneName);
	bool SaveScene(const char* sceneName);
	bool SaveSceneTGP(const char* sceneName);
	void UnloadScene();

	void RemoveGameObject(std::string gameObjectLabel);

	void UpdateImGui();

	GameObject* GetGameObject(std::string label);

	void DrawAxisForObject(GameObject* gameObject, const XMMATRIX& viewProjection);

	AxisEditSubState GetAxisEditSubState();
	void StopAxisEdit();

	static XMVECTOR RayPlaneIntersect(XMVECTOR rayPoint, XMVECTOR rayDirection, XMVECTOR planeNormal, XMVECTOR planePoint);

	Camera camera;
	RenderableGameObject skybox;
	RenderableGameObject clouds;
	RenderableGameObject ocean;
	Light directionalLight;

	std::unordered_map<std::string, GameObject*> gameObjectMap;
	std::vector<RenderableGameObject*> renderableGameObjects;
	std::vector<PhysicsGameObject*> physicsGameObjects;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;

	Model axisTranslateModel;
	Model axisRotateModel;

	XMFLOAT3 xAxisTranslateDefaultBounds;
	XMFLOAT3 yAxisTranslateDefaultBounds;
	XMFLOAT3 zAxisTranslateDefaultBounds;

	BoundingBox xAxisTranslateBoudingBox;
	BoundingBox yAxisTranslateBoudingBox;
	BoundingBox zAxisTranslateBoudingBox;

	ResourceManager resourceManager;
	ControllerManager* controllerManager;

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void FloatObject(GameObject* object);
	float GetWaterHeightAt(float posX, float posZ);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexShader;
	VertexShader waterVertexShader;
	PixelShader pixelShader;
	PixelShader noLightPixelShader;
	PixelShader cloudsPixelShader;

	ConstantBuffer<CB_VS_vertexShader> cb_vs_vertexShader;
	ConstantBuffer<CB_PS_pixelShader> cb_ps_pixelShader;
	ConstantBuffer<CB_PS_noLightPixelShader> cb_ps_noLightPixelShader;
	ConstantBuffer<CB_PS_cloudsPixelShader> cb_ps_cloudsPixelShader;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> regularRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unordered_map<std::string, ObjectTrack*> objectTracks;
	Texture* noiseTexture;

	bool selectingGameObject = false;
	RenderableGameObject* selectedObject;
	AxisEditState axisEditState = AxisEditState::EDIT_TRANSLATE;
	AxisEditSubState axisEditSubState = AxisEditSubState::EDIT_NONE;
	float lastAxisGrabOffset = FLT_MAX;
	XMVECTOR lastGrabPos;

	int windowWidth = 0;
	int windowHeight = 0;

	int mousePosX = 0;
	int mousePosY = 0;

	float mouseNDCX = 0.0f;
	float mouseNDCY = 0.0f;

	Timer fpsTimer;
	int fpsCounter;
	std::string fpsString;

	bool useWireframe = false;
	bool useVSync = true;

	bool sceneLoaded = false;

	int newObjectType;
	bool newObjectMenuOpen = false;
	std::string newObjectLabel = "";
	std::string newObjectModelPath = "";
	const std::string defaultObjectModelFilepath = "res/models/";
};