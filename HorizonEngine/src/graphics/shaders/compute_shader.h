#pragma once

#include "i_shader.h"

namespace hrzn::gfx
{
	class ComputeShader : public IShader
	{
	public:
		bool                 compileSourceInit(const char* sourcePath);
		bool                 loadCompiledInit(const char* compiledShaderPath);

		ID3D11ComputeShader* getShader();

	private:
		bool                 sharedInit();

	private:
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_shader;
	};
}
