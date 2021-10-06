
//Function implementations for the AdapterReader and AdapterData classes

#include "adapter_reader.h"

namespace hrzn::utils
{
	std::vector<AdapterData> AdapterReader::m_adapters;

	std::vector<AdapterData> AdapterReader::getAdapters()
	{
		if (m_adapters.size() > 0)
		{
			return m_adapters;
		}

		Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));

		if (FAILED(hr))
		{
			ErrorLogger::log(hr, "Failed to create DXGIFactory for enumerating adapters.");
			exit(-1);
		}

		IDXGIAdapter* pAdapter;
		UINT index = 0;

		while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
		{
			m_adapters.push_back(AdapterData(pAdapter));
			index++;
		}

		return m_adapters;
	}

	AdapterData::AdapterData(IDXGIAdapter* pAdapter) :
		m_pAdapter(pAdapter),
		m_description()
	{
		m_pAdapter = pAdapter;
		HRESULT hr = m_pAdapter->GetDesc(&m_description);

		if (FAILED(hr))
		{
			ErrorLogger::log(hr, "Failed to get description for IDXGIAdapter.");
		}
	}
}
