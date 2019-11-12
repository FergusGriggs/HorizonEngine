#pragma once

#include "AdapterReader.h"
#include "Shader.h"
#include "ConstantBufferTypes.h"
#include "Camera.h"
#include "../Timer.h"
#include "RenderableGameObject.h"
#include "Light.h"
#include "SpotLight.h"
#include "PointLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>

#include <WICTextureLoader.h>

#include <unordered_map>

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
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame(float deltaTime);
	void Update(float deltaTime);

	void AdjustMouseX(int xPos);
	void AdjustMouseY(int yPos);
	void SetMouseX(int xPos);
	void SetMouseY(int yPos);
	void ComputeMouseNDC();

	void CheckSelectingObject();
	void SlideSelectedObject();

	void UpdateImGui();

	void DrawAxisForObject(GameObject* gameObject, const XMMATRIX& viewProjection);

	AxisEditSubState GetAxisEditSubState();
	void StopAxisEdit();

	static XMVECTOR RayPlaneIntersect(XMVECTOR rayPoint, XMVECTOR rayDirection, XMVECTOR planeNormal, XMVECTOR planePoint);

	Camera camera;
	RenderableGameObject nano;
	RenderableGameObject woman;
	RenderableGameObject ocean;
	RenderableGameObject island;

	Light directionalLight;
	SpotLight spotLight;
	PointLight pointLight;

	Model axisTranslateModel;
	Model axisRotateModel;

	BoundingBox xAxisTranslateBoudingBox;
	BoundingBox yAxisTranslateBoudingBox;
	BoundingBox zAxisTranslateBoudingBox;


private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void InitializeTracks();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexShader;
	VertexShader waterVertexShader;
	PixelShader pixelShader;
	PixelShader noLightPixelShader;

	ConstantBuffer<CB_VS_vertexShader> cb_vs_vertexShader;
	ConstantBuffer<CB_PS_pixelShader> cb_ps_pixelShader;
	ConstantBuffer<CB_PS_noLightPixelShader> cb_ps_noLightPixelShader;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> regularRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture2;

	std::unordered_map<std::string, ObjectTrack*> objectTracks;

	bool selectingGameObject = false;
	RenderableGameObject* selectedObject;
	AxisEditState axisEditState = AxisEditState::EDIT_TRANSLATE;
	AxisEditSubState axisEditSubState = AxisEditSubState::EDIT_NONE;
	float lastAxisGrabOffset = FLT_MAX;

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
};