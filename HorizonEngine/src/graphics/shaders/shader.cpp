
//Function implementations for the VertexShader, PixelShader and Compute Shader classes

#include "shader.h"

#include "../graphics_handler.h"

namespace hrzn::gfx
{
	bool VertexShader::initialise(std::wstring shaderFilePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
	{
		m_path = shaderFilePath;

		HRESULT hr = D3DReadFileToBlob(shaderFilePath.c_str(), m_shaderBuffer.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to read shader at path: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		hr = GraphicsHandler::it().getDevice()->CreateVertexShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to create vertex shader from: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		hr = GraphicsHandler::it().getDevice()->CreateInputLayout(layoutDesc, numElements, m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf());

		if (FAILED(hr))
		{
			utils::ErrorLogger::log("Failed to create shader input layout.");
			return false;
		}

		return true;
	}

	ID3D11VertexShader* VertexShader::getShader()
	{
		return m_shader.Get();
	}

	ID3D10Blob* VertexShader::getBuffer()
	{
		return m_shaderBuffer.Get();
	}

	ID3D11InputLayout* VertexShader::getInputLayout()
	{
		return m_inputLayout.Get();
	}

	bool PixelShader::initialise(std::wstring shaderFilePath)
	{
		m_path = shaderFilePath;

		HRESULT hr = D3DReadFileToBlob(shaderFilePath.c_str(), m_shaderBuffer.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to read shader at path: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		hr = GraphicsHandler::it().getDevice()->CreatePixelShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to create pixel shader from: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		return true;
	}

	ID3D11PixelShader* PixelShader::getShader()
	{
		return m_shader.Get();
	}

	ID3D10Blob* PixelShader::getBuffer()
	{
		return m_shaderBuffer.Get();
	}

	bool ComputeShader::initialise(std::wstring shaderFilePath)
	{
		m_path = shaderFilePath;

		HRESULT hr = D3DReadFileToBlob(shaderFilePath.c_str(), m_shaderBuffer.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to read shader at path: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		hr = GraphicsHandler::it().getDevice()->CreateComputeShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMessage = L"Failed to create pixel shader from: ";
			errorMessage += shaderFilePath;
			utils::ErrorLogger::log(hr, errorMessage);
			return false;
		}

		return true;
	}

	ID3D11ComputeShader* ComputeShader::getShader()
	{
		return m_shader.Get();
	}

	ID3D10Blob* ComputeShader::getBuffer()
	{
		return m_shaderBuffer.Get();
	}
}