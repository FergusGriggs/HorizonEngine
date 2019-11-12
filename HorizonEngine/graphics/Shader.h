#pragma once

#include "../ErrorLogger.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
	//ID3D11InputLayout* GetInputLayout();

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
	//Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};