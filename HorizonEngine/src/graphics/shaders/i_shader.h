
//Stores definitions for the VertexShader and PixelShader classes

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>

namespace hrzn::gfx
{
	class IShader
	{
	public:
		IShader();
		virtual ~IShader();

		bool         compileSourceInit(const char* sourcePath, const char* entryPoint, const char* profile);
		bool         loadCompiledInit(const char* shaderFilePath);

		ID3D10Blob*  getBuffer();

	protected:
		Microsoft::WRL::ComPtr<ID3D10Blob> m_shaderBuffer;
		std::string                        m_path;
	};
}
