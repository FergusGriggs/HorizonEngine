
#include "i_shader.h"

#include <d3dcompiler.h>

#include "../graphics_handler.h"
#include "../../utils/string_helpers.h"

namespace hrzn::gfx
{
	IShader::IShader() :
		m_shaderBuffer(nullptr),
		m_path("")
	{
	}

	IShader::~IShader()
	{
	}

    bool IShader::compileSourceInit(const char* sourcePath, const char* entryPoint, const char* profile)
    {
        if (!sourcePath || !entryPoint || !profile)
        {
            std::string errorMessage = "Failed to load/compile shader at path: ";
            if (sourcePath)
            {
                errorMessage += sourcePath;
            }
            (errorMessage);
            return false;
        }

        m_path = sourcePath;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif

        const D3D_SHADER_MACRO defines[] =
        {
            "EXAMPLE_DEFINE", "1", NULL, NULL
        };

        ID3DBlob* errorBlob = nullptr;
        HRESULT hr = D3DCompileFromFile(utils::string_helpers::stringToWide(m_path).c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint, profile, flags, 0, m_shaderBuffer.GetAddressOf(), &errorBlob);

        std::string errorString;
        if (errorBlob)
        {
            errorString = (char*)errorBlob->GetBufferPointer();
            errorBlob->Release();
        }

        if (utils::ErrorLogger::logIfFailed(hr, "Failed to read/compile shader source at path: " + m_path + "\nShader Error: " + errorString))
        {
            return false;
        }

        return true;
    }

    bool IShader::loadCompiledInit(const char* shaderFilePath)
    {
        m_path = shaderFilePath;

        HRESULT hr = D3DReadFileToBlob(utils::string_helpers::stringToWide(shaderFilePath).c_str(), m_shaderBuffer.GetAddressOf());

        if (FAILED(hr))
        {
            std::string errorMessage = "Failed to read pre-compiled shader at path: ";
            errorMessage += shaderFilePath;
            utils::ErrorLogger::log(hr, errorMessage);
            return false;
        }

        return true;
    }

	ID3D10Blob* IShader::getBuffer()
	{
		return m_shaderBuffer.Get();
	}
}