#include "vertex_shader.h"

#include <d3dcompiler.h>
#include <d3d11shader.h>

#pragma comment(lib, "dxguid.lib")

#include "../graphics_handler.h"

namespace hrzn::gfx
{
	bool VertexShader::compileSourceInit(const char* sourcePath)
	{
        if (!IShader::compileSourceInit(sourcePath, "main", "vs_5_0"))
        {
            return false;
        }

		return sharedInit();
	}

    bool VertexShader::loadCompiledInit(const char* compiledShaderPath)
    {
        if (!IShader::loadCompiledInit(compiledShaderPath))
        {
            return false;
        }

        return sharedInit();
    }

    bool VertexShader::sharedInit()
    {
        HRESULT hr;
        hr = GraphicsHandler::it().getDevice()->CreateVertexShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());
        if (utils::ErrorLogger::logIfFailed(hr, "Failed to create ID3D11VertexShader for: " + m_path))
        {
            return false;
        }

        return generateInputLayoutFromShaderBlob();
    }

    bool VertexShader::generateInputLayoutFromShaderBlob()
	{
        // Reflect shader info
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflection;
        HRESULT hr = D3DReflect(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)vertexShaderReflection.GetAddressOf());
        if (utils::ErrorLogger::logIfFailed(hr, "Failed to generate input layout for vertex shader because D3DReflect failed. Shader path: " + m_path))
        {
            return false;
        }

        // Get shader info
        D3D11_SHADER_DESC shaderDesc;
        vertexShaderReflection->GetDesc(&shaderDesc);

        // Read input layout description from shader info
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescriptions;
        for (unsigned int i = 0; i < shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

            // Fill out input element desc
            D3D11_INPUT_ELEMENT_DESC elementDesc;
            elementDesc.SemanticName = paramDesc.SemanticName;
            elementDesc.SemanticIndex = paramDesc.SemanticIndex;
            elementDesc.InputSlot = 0;
            elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate = 0;

            // Determine DXGI format
            if (paramDesc.Mask == 1)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
            }
            else if (paramDesc.Mask <= 3)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
            }
            else if (paramDesc.Mask <= 7)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if (paramDesc.Mask <= 15)
            {
                if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            // Save element desc
            inputLayoutDescriptions.push_back(elementDesc);
        }

        // Try to create Input Layout
        hr = GraphicsHandler::it().getDevice()->CreateInputLayout(&inputLayoutDescriptions[0], (UINT)inputLayoutDescriptions.size(), m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), m_inputLayout.GetAddressOf());
        if (utils::ErrorLogger::logIfFailed(hr, "Failed to create input layout for vertex shader at path: " + m_path))
        {
            return false;
        }

        return true;
	}
    
    ID3D11VertexShader* VertexShader::getShader()
	{
		return m_shader.Get();
	}

	ID3D11InputLayout* VertexShader::getInputLayout()
	{
		return m_inputLayout.Get();
	}
}
