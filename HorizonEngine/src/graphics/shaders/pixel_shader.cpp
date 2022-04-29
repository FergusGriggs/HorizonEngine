#include "pixel_shader.h"

#include "../graphics_handler.h"

namespace hrzn::gfx
{
    bool PixelShader::compileSourceInit(const char* sourcePath)
    {
        if (!IShader::compileSourceInit(sourcePath, "main", "ps_5_0"))
        {
            return false;
        }

        return sharedInit();
    }

    bool PixelShader::loadCompiledInit(const char* compiledShaderPath)
    {
        if (!IShader::loadCompiledInit(compiledShaderPath))
        {
            return false;
        }

        return sharedInit();
    }

    ID3D11PixelShader* PixelShader::getShader()
    {
        return m_shader.Get();
    }

    bool PixelShader::sharedInit()
    {
		HRESULT hr;
		hr = GraphicsHandler::it().getDevice()->CreatePixelShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());
		if (utils::ErrorLogger::logIfFailed(hr, "Failed to create ID3D11PixelShader for: " + m_path))
		{
			return false;
		}

		return true;
    }
}