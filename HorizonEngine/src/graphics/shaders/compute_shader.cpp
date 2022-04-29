#include "compute_shader.h"

#include "../graphics_handler.h"

#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	bool ComputeShader::compileSourceInit(const char* sourcePath)
	{
		if (!IShader::compileSourceInit(sourcePath, "CSMain", "cs_5_0"))
		{
			return false;
		}

		return sharedInit();
	}

	bool ComputeShader::loadCompiledInit(const char* compiledShaderPath)
	{
		if (!IShader::loadCompiledInit(compiledShaderPath))
		{
			return false;
		}

		return sharedInit();
	}

	ID3D11ComputeShader* ComputeShader::getShader()
	{
		return m_shader.Get();
	}

	bool ComputeShader::sharedInit()
	{
		HRESULT hr;
		hr = GraphicsHandler::it().getDevice()->CreateComputeShader(m_shaderBuffer->GetBufferPointer(), m_shaderBuffer->GetBufferSize(), NULL, m_shader.GetAddressOf());
		if (utils::ErrorLogger::logIfFailed(hr, "Failed to create ID3D11ComputeShader for: " + m_path))
		{
			return false;
		}

		return true;
	}
}
