
//Stores definitions for the VertexShader and PixelShader classes

#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	class VertexShader
	{
	public:
		bool initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
		ID3D11VertexShader* getShader();
		ID3D10Blob* getBuffer();
		ID3D11InputLayout* getInputLayout();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
		Microsoft::WRL::ComPtr<ID3D10Blob>         m_shaderBuffer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
	};

	class PixelShader
	{
	public:
		bool initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath);
		ID3D11PixelShader* getShader();
		ID3D10Blob* getBuffer();

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
		Microsoft::WRL::ComPtr<ID3D10Blob>        m_shaderBuffer;
	};

	class ComputeShader
	{
	public:
		bool initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath);
		ID3D11ComputeShader* getShader();
		ID3D10Blob* getBuffer();

	private:
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_shader;
		Microsoft::WRL::ComPtr<ID3D10Blob>          m_shaderBuffer;
	};
}
