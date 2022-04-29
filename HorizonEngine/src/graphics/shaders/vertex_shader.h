#pragma once

#include "i_shader.h"

namespace hrzn::gfx
{
	class VertexShader : public IShader
	{
	public:
		bool                compileSourceInit(const char* sourcePath);
		bool                loadCompiledInit(const char* compiledShaderPath);

		ID3D11VertexShader* getShader();
		ID3D11InputLayout*  getInputLayout();

	private:
		bool                sharedInit();
		bool                generateInputLayoutFromShaderBlob();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
	};
}
