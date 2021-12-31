#include "geometry_buffer.h"

#include "../../user_config.h"
#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	GeometryBuffer::GeometryBuffer()
	{
	}

	GeometryBuffer::~GeometryBuffer()
	{
	}

	void GeometryBuffer::initialise(UINT texWidth, UINT texHeight)
	{
		m_albedo.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight);
		m_positionRoughness.initialise(DXGI_FORMAT_R16G16B16A16_FLOAT, texWidth, texHeight);
		m_normalAO.initialise(DXGI_FORMAT_R16G16B16A16_FLOAT, texWidth, texHeight);
		m_emissionMetallic.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight);
		m_depthStencil.initialise(DXGI_FORMAT_R32_FLOAT, texWidth, texHeight);

		// Fill geometry buffer render target array
		m_renderTargetViews[0] = m_albedo.m_renderTargetView.Get();
		m_renderTargetViews[1] = m_positionRoughness.m_renderTargetView.Get();
		m_renderTargetViews[2] = m_normalAO.m_renderTargetView.Get();
		m_renderTargetViews[3] = m_emissionMetallic.m_renderTargetView.Get();

		// Fill geometry buffer shader resource view array
		m_shaderResourceViews[0] = m_albedo.m_shaderResourceView.Get();
		m_shaderResourceViews[1] = m_positionRoughness.m_shaderResourceView.Get();
		m_shaderResourceViews[2] = m_normalAO.m_shaderResourceView.Get();
		m_shaderResourceViews[3] = m_emissionMetallic.m_shaderResourceView.Get();
	}

	void GeometryBuffer::release()
	{
		m_albedo.release();
		m_positionRoughness.release();
		m_normalAO.release();
		m_emissionMetallic.release();
		m_depthStencil.release();

		for (int RTVIndex = 0; RTVIndex < 4; ++RTVIndex)
		{
			m_renderTargetViews[RTVIndex] = nullptr;
		}
		
		for (int SRVIndex = 0; SRVIndex < 4; ++SRVIndex)
		{
			m_shaderResourceViews[SRVIndex] = nullptr;
		}
	}
}
