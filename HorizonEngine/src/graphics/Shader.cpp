//Shader.cpp
//Function implementations for the VertexShader and PixelShader classes

#include "Shader.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderFilePath.c_str(), this->shaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMessage = L"Failed to read shader at path: ";
		errorMessage += shaderFilePath;
		ErrorLogger::Log(hr, errorMessage);
		return false;
	}

	hr = device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMessage = L"Failed to create vertex shader from: ";
		errorMessage += shaderFilePath;
		ErrorLogger::Log(hr, errorMessage);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());

	if (FAILED(hr)) {
		ErrorLogger::Log("Failed to create shader input layout.");
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderFilePath)
{
	HRESULT hr = D3DReadFileToBlob(shaderFilePath.c_str(), this->shaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMessage = L"Failed to read shader at path: ";
		errorMessage += shaderFilePath;
		ErrorLogger::Log(hr, errorMessage);
		return false;
	}

	hr = device->CreatePixelShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMessage = L"Failed to create pixel shader from: ";
		errorMessage += shaderFilePath;
		ErrorLogger::Log(hr, errorMessage);
		return false;
	}

}

ID3D11PixelShader* PixelShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
	return this->shaderBuffer.Get();
}