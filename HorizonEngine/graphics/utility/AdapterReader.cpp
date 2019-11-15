#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters() {
	if (adapters.size() > 0) {
		return adapters;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;

	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter))) {
		adapters.push_back(AdapterData(pAdapter));
		index++;
	}

	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter* pAdapter) {
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&this->description);

	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to get description for IDXGIAdapter.");
	}
}