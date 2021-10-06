
//Retrieves and stores information about the available graphics adapters

#pragma once

#include "error_logger.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

namespace hrzn::utils
{
	class AdapterData
	{
	public:
		AdapterData(IDXGIAdapter* pAdapter);

		IDXGIAdapter*     m_pAdapter;
		DXGI_ADAPTER_DESC m_description;
	};

	class AdapterReader
	{
	public:
		static std::vector<AdapterData> getAdapters();

	private:
		static std::vector<AdapterData> m_adapters;
	};
}