//main.cpp
//Application entry point, creates and runs instance of Horizon class

#include "Horizon.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to co-initialize.");
	}

	Horizon engine = Horizon();
	if (engine.Initialize(hInstance, "HorizonEngine", "windowClass", 1280, 720)) {
		while (engine.ProcessMessages()) {
			engine.Update();
			engine.RenderFrame();
		}
	
	}

	return 0;
}