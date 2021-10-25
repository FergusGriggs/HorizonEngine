
//Application entry point, creates and runs instance of Horizon class

#include "horizon.h"

#include <iostream>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr))
	{
		hrzn::utils::ErrorLogger::log(hr, "Failed to co-initialize.");
	}

	hrzn::Horizon engine = hrzn::Horizon();
	if (engine.initialize(hInstance, "windowClass"))
	{
		while (engine.processMessages())
		{
			engine.update();
			engine.renderFrame();
		}
	
	}

	return 0;
}