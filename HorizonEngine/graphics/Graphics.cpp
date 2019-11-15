#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height) {

	fpsTimer.Start();

	this->windowWidth = width;
	this->windowHeight = height;

	if (!InitializeDirectX(hwnd)) {
		return false;
	}
	if (!InitializeShaders()) {
		return false;
	}

	//INIT OBJECT TRACK DATA
	InitializeTracks();

	//INIT SCENE OBJECTS
	if (!InitializeScene()) {
		return false;
	}

	//INIT IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

bool Graphics::InitializeShaders() {

	std::wstring shaderFolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderFolder = L"../x64/Debug/";
#else  //x86 (Win32)
		shaderFolder = L"../Debug/";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		shaderFolder = L"../x64/Release/";
#else  //x86 (Win32)
		shaderFolder = L"../Release/";
#endif
#endif
	}

	//CREATE VERTEX SHADER INPUT LAYOUT
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	//INITIALIZE VERTEX SHADERS
	if (!vertexShader.Initialize(this->device, shaderFolder + L"vertexShader.cso", layout, numElements)) {
		return false;
	}

	if (!waterVertexShader.Initialize(this->device, shaderFolder + L"waterVertexShader.cso", layout, numElements)) {
		return false;
	}

	//INITIALIZE PIXEL SHADERS
	if (!pixelShader.Initialize(this->device, shaderFolder + L"pixelShader.cso")) {
		return false;
	}

	if (!noLightPixelShader.Initialize(this->device, shaderFolder + L"noLightPixelShader.cso")) {
		return false;
	}
}

bool Graphics::InitializeScene()
{
	try
	{
		//LOAD TEXTURES
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"res\\textures\\me.png", nullptr, diffuseTexture.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"res\\textures\\pink.jpg", nullptr, diffuseTexture2.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from file.");

		//CREATE CONSTANT BUFFERS
		hr = this->cb_vs_vertexShader.Initialize(this->device.Get(), this->deviceContext.Get());

		COM_ERROR_IF_FAILED(hr, "Failed to create 'cb_vs_vertexShader' constant buffer.");

		hr = this->cb_ps_pixelShader.Initialize(this->device.Get(), this->deviceContext.Get());

		COM_ERROR_IF_FAILED(hr, "Failed to create 'cb_ps_pixelShader' constant buffer.");

		hr = this->cb_ps_noLightPixelShader.Initialize(this->device.Get(), this->deviceContext.Get());

		COM_ERROR_IF_FAILED(hr, "Failed to create 'cb_ps_noLightPixelShader' constant buffer.");

		this->cb_ps_pixelShader.data.useNormalMapping = true;

		//LOAD AXIS MODELS
		if (!this->axisTranslateModel.Initialize("res/models/axis/translate2.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		if (!this->axisRotateModel.Initialize("res/models/axis/rotate2.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		//SET TRANSLATE AXIS DEFAULT BOUNDS / EXTENTS
		xAxisTranslateDefaultBounds = XMFLOAT3(0.45f, 0.05f, 0.05f);
		yAxisTranslateDefaultBounds = XMFLOAT3(0.05f, 0.45f, 0.05f);
		zAxisTranslateDefaultBounds = XMFLOAT3(0.05f, 0.05f, 0.45f);

		//INITIALIZE RENDERABLES
		if (!woman.Initialize("Lady", "res/models/photoscan/photoscan.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {//"nanosuit/nanosuit.obj"//tests/dodge_challenger.fbx//lambo/lambo.obj
			return false;
		}

		if (!nano.Initialize("Man", "res/models/man/man.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {//"nanosuit/nanosuit.obj"//tests/dodge_challenger.fbx//lambo/lambo.obj//house/Alpine_chalet.blend
			return false;
		}

		if (!ocean.Initialize("Ocean", "res/models/sea.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {//"nanosuit/nanosuit.obj"//tests/dodge_challenger.fbx//lambo/lambo.obj//house/Alpine_chalet.blend
			return false;
		}

		if (!island.Initialize("Island", "res/models/island/island.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		if (!boat.Initialize("Boat", "res/models/boat.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		//INITIALIZE LIGHTS
		if (!directionalLight.Initialize("Directional Light", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		if (!pointLight.Initialize("Point Light", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		if (!spotLight.Initialize("Spot Light", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexShader)) {
			return false;
		}

		//ASSIGN OBJECT PROPERTIES
		boat.SetObjectTrack(objectTracks.at("boat_track"));
		boat.SetFollowingObjectTrack(true);

		island.SetPosition(0.0f, -5.0f, 0.0f);

		woman.SetPosition(2.0f, 7.0f, 0.0f);
		woman.SetObjectTrack(objectTracks.at("lady_track"));
		woman.SetObjectTrackDelta(2.0f);
		woman.SetFollowingObjectTrack(true);

		nano.SetPosition(-2.0f, 7.0f, 0.0f);
		nano.SetObjectTrack(objectTracks.at("man_track"));
		nano.SetFollowingObjectTrack(true);

		directionalLight.SetPosition(2.0f, 5.0f, 2.0f);
		directionalLight.SetLookAtPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
		directionalLight.SetColour(XMFLOAT3(0.8f, 0.8f, 0.8f));

		spotLight.SetPosition(-10.0f, 10.0f, -10.0f);
		spotLight.SetObjectTrack(objectTracks.at("spot_light_track"));
		spotLight.SetFollowingObjectTrack(true);

		pointLight.SetPosition(0.0f, 8.0f, 0.0f);
		pointLight.SetObjectTrack(objectTracks.at("point_light_track"));
		pointLight.SetFollowingObjectTrack(true);

		camera.SetPosition(0.0f, 10.0f, -7.0f);
		camera.SetLookAtPos(XMFLOAT3(0.0f, 7.0f, 0.0f));
		camera.SetProjectionValues(40.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 1000.0f);
		camera.SetObjectTrack(objectTracks.at("camera_track"));
		camera.SetFollowingObjectTrack(true);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::RenderFrame(float deltaTime) {
	//UPDATE THE PIXEL SHADER CONSTANT BUFFER
	pointLight.UpdateShaderVariables(this->cb_ps_pixelShader);
	directionalLight.UpdateShaderVariables(this->cb_ps_pixelShader);
	spotLight.UpdateShaderVariables(this->cb_ps_pixelShader);
	
	this->cb_ps_pixelShader.data.cameraPosition = camera.GetPositionFloat3();

	this->cb_ps_pixelShader.MapToGPU();
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_pixelShader.GetAddressOf());

	//CLEAR RENDER TARGET VIEW AND DEPTH STENCIL VIEW
	float backgroundColour[] = { 0.62f * this->directionalLight.colour.x, 0.90 * this->directionalLight.colour.y, 1.0f * this->directionalLight.colour.z, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), backgroundColour);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//UPDATE INPUT ASSEMBLER
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//SET RASTERIZER STATE
	if (useWireframe) {
		this->deviceContext->RSSetState(this->wireframeRasterizerState.Get());
	}
	else
	{
		this->deviceContext->RSSetState(this->regularRasterizerState.Get());
	}

	//SET DEPTH STENCIL STATE
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	//SET BLEND STATE
	this->deviceContext->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFFFF);//this->blendState.Get()

	//SET SAMPLER STATE
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	//SET VERTEX AND PIXEL SHADERS
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);

	UINT offset = 0;

	this->cb_vs_vertexShader.data.gameTime += deltaTime;

	XMMATRIX viewProjMat = camera.GetViewMatrix() * camera.GetProjectionMatrix();

	{
		//DRAW REGULAR GAME OBJECTS
		this->nano.Draw(viewProjMat);
		this->woman.Draw(viewProjMat);
		this->island.Draw(viewProjMat);
		this->boat.Draw(viewProjMat);

		//DRAW WATER
		this->deviceContext->VSSetShader(waterVertexShader.GetShader(), NULL, 0);

		this->ocean.Draw(viewProjMat);

		this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	}
	
	{
		//DRAW NO LIGHT OBJECTS
		this->deviceContext->PSSetShader(noLightPixelShader.GetShader(), NULL, 0);

		this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_noLightPixelShader.GetAddressOf());

		this->cb_ps_noLightPixelShader.data.colour = this->pointLight.colour;
		this->cb_ps_noLightPixelShader.MapToGPU();
		this->pointLight.Draw(viewProjMat);

		this->cb_ps_noLightPixelShader.data.colour = this->spotLight.colour;
		this->cb_ps_noLightPixelShader.MapToGPU();
		this->spotLight.Draw(viewProjMat);

		this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		if (selectingGameObject) {
			this->cb_ps_noLightPixelShader.data.colour = XMFLOAT3(1.0f, 1.0f, 1.0f);
			this->cb_ps_noLightPixelShader.MapToGPU();
			DrawAxisForObject(selectedObject, viewProjMat);
		}
	}

	//UPDATE FPS TIMER
	fpsCounter++;
	if (fpsTimer.GetMillisecondsElapsed() > 1000.0) {
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}

	//RENDER UI (NOT IMGUI)
	spriteBatch->Begin();

	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(1.0f, 0.5f), DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	
	if (selectingGameObject) {
		XMFLOAT2 screenNDC = camera.GetNDCFrom3DPos(selectedObject->GetPositionVector() + XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f));//XMFLOAT2(-0.5,0.5);
		XMFLOAT2 screenPos = DirectX::XMFLOAT2((screenNDC.x * 0.5f + 0.5f) * windowWidth, (1.0f - (screenNDC.y * 0.5f + 0.5f)) * windowHeight);
		XMVECTOR size = spriteFont->MeasureString(StringHelper::StringToWide(selectedObject->GetLabel()).c_str());
		screenPos.x -= XMVectorGetX(size) * 0.5f;
		screenPos.y -= XMVectorGetY(size) * 0.5f;
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(selectedObject->GetLabel()).c_str(), screenPos, DirectX::Colors::Black, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		screenPos.y -= 0.5f;
		screenPos.x -= 1.0f;
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(selectedObject->GetLabel()).c_str(), screenPos, DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	}

	spriteBatch->End();

	ImGui::Render();

	//RENDER IMGUI
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//PRESENT IMAGE
	this->swapChain->Present(useVSync, NULL);// using vsync
}

void Graphics::UpdateSelectedObject() {
	//CHECK MOUSE IS ON SCREEN
	if (mouseNDCX > -1.0f && mouseNDCX < 1.0f && mouseNDCY > -1.0f && mouseNDCY < 1.0f) {
		//TRANSLATING
		if (this->axisEditState == AxisEditState::EDIT_TRANSLATE) {
			XMFLOAT3 objectPos = selectedObject->GetPositionFloat3();
			switch (this->axisEditSubState) {
			case AxisEditSubState::EDIT_X:
			{
				//GET INTERSECT POINT WITH MOUSE RAY
				XMVECTOR intersect = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				float currentAxisGrabOffset = XMVectorGetX(intersect); //GET RELEVANT VECTOR COMPONENT
				if (this->lastAxisGrabOffset != FLT_MAX) { //CHECK IF FIRST ITERATION
					float diff = currentAxisGrabOffset - this->lastAxisGrabOffset; //FIND DIFFERENCE
					selectedObject->AdjustPosition(diff, 0.0f, 0.0f); //MOVE OBJECT BY DIFFERENCE
				}
				this->lastAxisGrabOffset = currentAxisGrabOffset; //SET LAST OFFSET
				break;
			}
			case AxisEditSubState::EDIT_Y:
			{
				XMVECTOR intersect = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), XMVector3Normalize(XMVectorSet(camera.GetPositionFloat3().x - objectPos.x, 0.0f, camera.GetPositionFloat3().z - objectPos.z, 0.0f)), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				float currentAxisGrabOffset = XMVectorGetY(intersect);
				if (this->lastAxisGrabOffset != FLT_MAX) {
					float diff = currentAxisGrabOffset - this->lastAxisGrabOffset;
					selectedObject->AdjustPosition(0.0f, diff, 0.0f);
				}
				this->lastAxisGrabOffset = currentAxisGrabOffset;
				break;
			}
			case AxisEditSubState::EDIT_Z:
			{
				XMVECTOR intersect = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				float currentAxisGrabOffset = XMVectorGetZ(intersect);
				if (this->lastAxisGrabOffset != FLT_MAX) {
					float diff = currentAxisGrabOffset - this->lastAxisGrabOffset;
					selectedObject->AdjustPosition(0.0f, 0.0f, diff);
				}
				this->lastAxisGrabOffset = currentAxisGrabOffset;
				break;
			}
			}
		}
		//ROTATING
		else if (this->axisEditState == AxisEditState::EDIT_ROTATE) {
			XMFLOAT3 objectPos = selectedObject->GetPositionFloat3();
			XMMATRIX modelRotationMatrix = selectedObject->GetRotationMatrix();
			XMMATRIX inverseModelRotationMatrix = XMMatrixInverse(nullptr, modelRotationMatrix);

			switch (this->axisEditSubState) {
			case AxisEditSubState::EDIT_X:
			{
				//COMPUTE WORLD SPACE AXIS VECTOR (PLANE NORMAL)
				XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), modelRotationMatrix));
				//GET INTERSECT POINT OF THIS PLANE WITH MOUSE RAY
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector(); //COMPUTE DIFFERENCE
				XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix); //TRANSFORM TO MODEL SPACE
				float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetZ(modelSpaceCentreDiff)); //WORK OUT ANGLE OF ROTATION
				if (this->lastAxisGrabOffset != FLT_MAX) { //IF NOT FIRST ITERATION
					float rotationDiff = rotation - this->lastAxisGrabOffset; //FIND ANGLE DIFF
					selectedObject->RotateAxisVectors(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationDiff); //ROTATE AXIS BY DIFFERENCE
					this->lastAxisGrabOffset = rotation - rotationDiff; //SET LAST OFFSET
				}
				else {
					this->lastAxisGrabOffset = rotation;
				}
				break;
			}
			case AxisEditSubState::EDIT_Y:
			{
				XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), modelRotationMatrix));
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector();
				XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
				float x = XMVectorGetX(modelSpaceCentreDiff);
				float y = XMVectorGetY(modelSpaceCentreDiff);
				float z = XMVectorGetZ(modelSpaceCentreDiff);
				float rotation = atan2(XMVectorGetZ(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
				if (this->lastAxisGrabOffset != FLT_MAX) {
					float rotationDiff = rotation - this->lastAxisGrabOffset;
					selectedObject->RotateAxisVectors(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationDiff);
					this->lastAxisGrabOffset = rotation - rotationDiff;
				}
				else {
					this->lastAxisGrabOffset = rotation;
				}
				break;
			}
			case AxisEditSubState::EDIT_Z:
			{
				XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), modelRotationMatrix));
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector();
				XMVECTOR modelSpaceCentreDiff = XMVector3Transform(centreDiff, inverseModelRotationMatrix);
				float rotation = atan2(XMVectorGetY(modelSpaceCentreDiff), XMVectorGetX(modelSpaceCentreDiff));
				if (this->lastAxisGrabOffset != FLT_MAX) {
					float rotationDiff = rotation - this->lastAxisGrabOffset;
					selectedObject->RotateAxisVectors(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), -rotationDiff);
					this->lastAxisGrabOffset = rotation - rotationDiff;
				}
				else {
					this->lastAxisGrabOffset = rotation;
				}
				break;
			}
			}
		}
	}
}

void Graphics::UpdateImGui()
{
	//START NEW FRAME
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//SELECTED OBJECT MENU
	if (selectingGameObject) {
		ImGui::Begin("Game Object Settings"); //TITLE
		ImGui::Text(("Label: " + selectedObject->GetLabel()).c_str()); //OBJECT LABEL
		XMFLOAT3 pos = selectedObject->GetPositionFloat3();
		ImGui::Text(("X: " + std::to_string(pos.x) + "Y: " + std::to_string(pos.y) + "Z: " + std::to_string(pos.z)).c_str()); //POSITION
		if (selectedObject->GetObjectTrack() != nullptr) { //FOLLOW TRACK CHECKBOX
			bool followingTrack = selectedObject->GetFollowingObjectTrack();
			ImGui::Checkbox("Follow Track", &followingTrack);
			selectedObject->SetFollowingObjectTrack(followingTrack);
		}
		if (ImGui::Button("Edit Translation")) {
			this->axisEditState = AxisEditState::EDIT_TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit Rotation")) {
			this->axisEditState = AxisEditState::EDIT_ROTATE;
		}
		if (ImGui::Button("Reset Rotation")) {
			this->selectedObject->SetRotation(0.0f, 0.0f, 0.0f);
		}
		GameObjectType type = selectedObject->GetType();
		if (type == GameObjectType::RENDERABLE) {//IF IT IS A RENDERABLE, ALLOW SCALING
			XMFLOAT3 scale = selectedObject->GetScale();
			ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.1f, 10.0f);
			selectedObject->SetScale(scale);
		}
		else if (type == GameObjectType::POINT_LIGHT || type == GameObjectType::SPOT_LIGHT) { // IF IT IS A LIGHT, ALLOW COLOUR CHANGE AND MOVE TO CAMERA
			Light* lightObj = reinterpret_cast<Light*>(selectedObject);
			ImGui::ColorEdit3("Colour", &(lightObj->colour.x));
			if (ImGui::Button("Move to Camera")) {
				XMVECTOR lightPosition = this->camera.GetPositionVector();
				lightPosition += this->camera.GetFrontVector();
				lightObj->SetPosition(lightPosition);
				lightObj->CopyAxisVectorsFrom(&camera);
				lightObj->SetFollowingObjectTrack(false);
			}
		}
		if (ImGui::Button("Close")) {
			selectedObject = nullptr;
			selectingGameObject = false;
		}
		ImGui::End();
	}

	ImGui::Begin("Scene Settings");
	
	//DIRECTIONAL LIGHT COLOUR
	ImGui::ColorEdit3("Dir Light Colour", &directionalLight.colour.x);
	float cameraTrackDelta = camera.GetObjectTrackDelta();
	ImGui::DragFloat("Camera Track Delta", &cameraTrackDelta, 0.005f, -0.5f, 10.0f);
	camera.SetObjectTrackDelta(cameraTrackDelta);

	//CAMERA TRACK CHECKBOX
	bool followTrack = camera.GetFollowingObjectTrack();
	ImGui::Checkbox("Camera Follow Track", &followTrack);
	camera.SetFollowingObjectTrack(followTrack);

	//NORMAL MAPPING CHECKBOX
	bool useNormalMapping = static_cast<bool>(this->cb_ps_pixelShader.data.useNormalMapping);
	ImGui::Checkbox("Normal Mapping", &useNormalMapping);
	this->cb_ps_pixelShader.data.useNormalMapping = useNormalMapping;

	ImGui::SameLine();

	//POM CHECKBOX
	bool useParallaxOcclusionMapping = static_cast<bool>(this->cb_ps_pixelShader.data.useParallaxOcclusionMapping);
	ImGui::Checkbox("PO Mapping", &useParallaxOcclusionMapping);
	this->cb_ps_pixelShader.data.useParallaxOcclusionMapping = useParallaxOcclusionMapping;

	ImGui::SameLine();

	//WIREFRAME CHECKBOX
	ImGui::Checkbox("Wireframe", &useWireframe);

	//POM HEIGHT CHECKBOX
	static float parallaxOcclusionMappingHeight = 0.025f;
	ImGui::DragFloat("PO Mapping Height", &parallaxOcclusionMappingHeight, 0.001f, 0.0f, 0.2f);
	this->cb_ps_pixelShader.data.parallaxOcclusionMappingHeight = parallaxOcclusionMappingHeight;

	//VSYNC CHECKBOX
	ImGui::Checkbox("Use VSync", &useVSync);

	ImGui::End();
}

void Graphics::Update(float deltaTime)
{
	//UPDATE MOUSE NDC
	this->ComputeMouseNDC();

	//UPDATE MOUSE TO WORLD VECTOR
	this->camera.ComputeMouseToWorldVectorDirection(mouseNDCX, mouseNDCY);

	//UPDATE SELECTED OBJECT (TRANSLATION / ROTATION)
	if (selectingGameObject) {
		UpdateSelectedObject();
	}

	//UPDATE GAME OBJECTS
	camera.Update(deltaTime);
	nano.Update(deltaTime);
	woman.Update(deltaTime);
	boat.Update(deltaTime * 0.25f);

	pointLight.Update(deltaTime);
	spotLight.Update(deltaTime);
	
	//UPDATE IMGUI
	UpdateImGui();
}

void Graphics::AdjustMouseX(int xPos)
{
	this->mousePosX += xPos;
}

void Graphics::AdjustMouseY(int yPos)
{
	this->mousePosY += yPos;
}

void Graphics::SetMouseX(int xPos)
{
	this->mousePosX = xPos;
}

void Graphics::SetMouseY(int yPos)
{
	this->mousePosY = yPos;
}

void Graphics::ComputeMouseNDC()
{
	mouseNDCX = (2.0f * static_cast<float>(mousePosX)) / (static_cast<float>(windowWidth)) - 1.0f;
	mouseNDCY = 1.0f - (2.0f * static_cast<float>(mousePosY)) / static_cast<float>(windowHeight);
}

bool Graphics::InitializeDirectX(HWND hwnd) {
	try
	{
		//GET GRAPHICS CARD ADAPTERS
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1) {
			ErrorLogger::Log("No DXGI Adapters Found.");
			return false;
		}

		//CREATE DEVICE AND SWAPCHAIN
		DXGI_SWAP_CHAIN_DESC swapChainDescription;
		ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDescription.BufferDesc.Width = this->windowWidth;
		swapChainDescription.BufferDesc.Height = this->windowHeight;
		swapChainDescription.BufferDesc.RefreshRate.Numerator = 120; //VSYNC FPS
		swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;

		swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDescription.SampleDesc.Count = 1; //NO ANTI ALIASING
		swapChainDescription.SampleDesc.Quality = 0; //LOWEST IMAGE QUALITY

		swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		swapChainDescription.BufferCount = 1;

		swapChainDescription.OutputWindow = hwnd;

		swapChainDescription.Windowed = TRUE;

		swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr = D3D11CreateDeviceAndSwapChain( adapters[0].pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
													NULL, NULL, NULL, 0, D3D11_SDK_VERSION, &swapChainDescription,
													this->swapChain.GetAddressOf(), this->device.GetAddressOf(),
													NULL, this->deviceContext.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		//GET BACKBUFFER
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

		hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

		COM_ERROR_IF_FAILED(hr, "Failed to get back buffer.");

		//CREATE RENDER TARGET VIEW
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//CREATE DEPTH STENCIL TEXTURE AND VIEW
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		//SET THE RENDER TARGET
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//CREATE DEPTH STENCIL STATE
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//CREATE VIEWPORT
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		this->deviceContext->RSSetViewports(1, &viewport);

		//CREATE DEFAULT RASTERIZER STATE
		D3D11_RASTERIZER_DESC regularRasterizerDesc;
		ZeroMemory(&regularRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		regularRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		regularRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		hr = this->device->CreateRasterizerState(&regularRasterizerDesc, this->regularRasterizerState.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create default rasterizer state.");

		//CREATE WIREFRAME RASTERIZER STATE
		D3D11_RASTERIZER_DESC wireFrameRasterizerDesc;
		ZeroMemory(&wireFrameRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		wireFrameRasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		wireFrameRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		hr = this->device->CreateRasterizerState(&wireFrameRasterizerDesc, this->wireframeRasterizerState.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create wireframe rasterizer state.");

		//CREATE DEFAULT BLEND STATE
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
		ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

		renderTargetBlendDesc.BlendEnable = true;
		renderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = renderTargetBlendDesc;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		//CREATE SPRITE BATCH AND SPRITE FONT INSTANCES
		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"res\\fonts\\consolas16.spritefont");//comicSansMS16.spritefont

		//CREATE SAMPLER STATE
		CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());

		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::InitializeTracks() {
	{
		//CREATE CAMERA TRACK
		ObjectTrack* cameraTrack = new ObjectTrack();

		//CREATE DATA
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 11.0f, -6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 11.0f, -6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(6.0f, 11.0f, -6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(6.0f, 11.0f, 0.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 11.0f, 6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 11.0f, 6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 11.0f, 0.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));
		cameraTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 11.0f, -6.0f), XMFLOAT3(0.0f, 3.5f, 0.0f)));

		cameraTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "camera_track", cameraTrack }); //INSERT INTO MAP
	}
	
	{
		//CREATE POINT TRACK
		ObjectTrack* pointLightTrack = new ObjectTrack();

		//CREATE DATA
		pointLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-1.5f, 5.5f, -1.5f), XMFLOAT3(-1.5f, 5.0f, -1.5f)));
		pointLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(1.5f, 3.5f, -1.5f), XMFLOAT3(1.5f, 3.0f, -1.5f)));
		pointLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(1.5f, 5.5f, 1.5f), XMFLOAT3(1.5f, 5.0f, 1.5f)));
		pointLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-1.5f, 3.5f, 1.5f), XMFLOAT3(-1.5f, 3.0f, 1.5f)));
		pointLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-1.5f, 5.5f, -1.5f), XMFLOAT3(-1.5f, 5.0f, -1.5f)));

		pointLightTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "point_light_track", pointLightTrack }); //INSERT INTO MAP
	}
	
	{
		//CREATE SPOT TRACK
		ObjectTrack* spotLightTrack = new ObjectTrack();

		//CREATE DATA
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 5.5f, -6.0f), XMFLOAT3(-5.0f, 4.5f, -5.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, -6.0f), XMFLOAT3(0.0f, 0.0f, -6.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(6.0f, 5.5f, -6.0f), XMFLOAT3(5.0f, 4.5f, -5.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(6.0f, 3.5f, 0.0f), XMFLOAT3(6.0f, 0.0f, 0.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(6.0f, 5.5f, 6.0f), XMFLOAT3(5.0f, 4.5f, 5.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, 6.0f), XMFLOAT3(0.0f, 0.0f, 6.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 5.5f, 6.0f), XMFLOAT3(-5.0f, 4.5f, 5.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 3.5f, 0.0f), XMFLOAT3(-6.0f, 0.0f, 0.0f)));
		spotLightTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-6.0f, 5.5f, -6.0f), XMFLOAT3(-5.0f, 4.5f, -5.0f)));

		spotLightTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "spot_light_track", spotLightTrack }); //INSERT INTO MAP
	}

	{
		//CREATE LADY TRACK
		ObjectTrack* ladyTrack = new ObjectTrack();

		//CREATE DATA
		ladyTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, 3.0f)));
		ladyTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, 3.0f), XMFLOAT3(3.0f, 3.5f, 0.0f)));
		ladyTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, -3.0f)));
		ladyTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, -3.0f), XMFLOAT3(-3.0f, 3.5f, 0.0f)));
		ladyTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, 3.0f)));

		ladyTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "lady_track", ladyTrack }); //INSERT INTO MAP
	}

	{
		//CREATE MAN TRACK
		ObjectTrack* manTrack = new ObjectTrack();

		//CREATE DATA
		manTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, 3.0f)));
		manTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, 3.0f), XMFLOAT3(3.0f, 3.5f, 0.0f)));
		manTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, -3.0f)));
		manTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, 3.5f, -3.0f), XMFLOAT3(-3.0f, 3.5f, 0.0f)));
		manTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-3.0f, 3.5f, 0.0f), XMFLOAT3(0.0f, 3.5f, 3.0f)));

		manTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "man_track", manTrack }); //INSERT INTO MAP
	}

	{
		//CREATE BOAT TRACK
		ObjectTrack* boatTrack = new ObjectTrack();

		//CREATE DATA
		boatTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-30.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, -3.5f, 30.0f)));
		boatTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, -3.5f, 30.0f), XMFLOAT3(30.0f, -3.5f, 0.0f)));
		boatTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(30.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, -3.5f, -30.0f)));
		boatTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(0.0f, -3.5f, -30.0f), XMFLOAT3(-30.0f, -3.5f, 0.0f)));
		boatTrack->AddTrackNode(ObjectTrackNode(XMFLOAT3(-30.0f, -3.5f, 0.0f), XMFLOAT3(0.0f, -3.5f, 30.0f)));

		boatTrack->GenerateMidPoints(); //INITIALIZE

		objectTracks.insert({ "boat_track", boatTrack }); //INSERT INTO MAP
	}
}

void Graphics::CheckSelectingObject()
{
	float closestDist = FLT_MAX;
	float distance;

	if (this->selectingGameObject) {
		XMFLOAT3 objectPos = this->selectedObject->GetPositionFloat3();

		float objectHitRadius = this->selectedObject->GetModel()->GetHitRadius();

		//CLAMP SCALE
		if (objectHitRadius < 0.75f) objectHitRadius = 0.75f;

		//TRANSLATE
		if (this->axisEditState == AxisEditState::EDIT_TRANSLATE) {
			objectHitRadius *= 2.0f;

			//UPDATE D3D COLLISION OBJECTS
			this->xAxisTranslateBoudingBox.Extents = XMFLOAT3(xAxisTranslateDefaultBounds.x * objectHitRadius, xAxisTranslateDefaultBounds.y * objectHitRadius, xAxisTranslateDefaultBounds.z * objectHitRadius);
			this->yAxisTranslateBoudingBox.Extents = XMFLOAT3(yAxisTranslateDefaultBounds.x * objectHitRadius, yAxisTranslateDefaultBounds.y * objectHitRadius, yAxisTranslateDefaultBounds.z * objectHitRadius);
			this->zAxisTranslateBoudingBox.Extents = XMFLOAT3(zAxisTranslateDefaultBounds.x * objectHitRadius, zAxisTranslateDefaultBounds.y * objectHitRadius, zAxisTranslateDefaultBounds.z * objectHitRadius);

			this->xAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x + 0.6f, objectPos.y, objectPos.z);
			this->yAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x, objectPos.y + 0.6f, objectPos.z);
			this->zAxisTranslateBoudingBox.Center = XMFLOAT3(objectPos.x, objectPos.y, objectPos.z + 0.6f);

			//CHECK IF AN AXIS IS CLICKED ON
			if (this->xAxisTranslateBoudingBox.Intersects(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), distance)) {
				if (distance < closestDist) {
					closestDist = distance;
					this->axisEditSubState = AxisEditSubState::EDIT_X;
				}
			}
			if (this->yAxisTranslateBoudingBox.Intersects(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), distance)) {
				if (distance < closestDist) {
					closestDist = distance;
					this->axisEditSubState = AxisEditSubState::EDIT_Y;
				}
			}
			if (this->zAxisTranslateBoudingBox.Intersects(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), distance)) {
				if (distance < closestDist) {
					closestDist = distance;
					this->axisEditSubState = AxisEditSubState::EDIT_Z;
				}
			}

			//STOP FOLLOWING TRACK AND RETURN IF AXIS SELECTED
			if (closestDist != FLT_MAX) {
				this->selectedObject->SetFollowingObjectTrack(false);
				return;
			}
		}
		//ROTATE
		else if (this->axisEditState == AxisEditState::EDIT_ROTATE) {

			XMMATRIX modelRotationMatrix = selectedObject->GetRotationMatrix();

			{
				//GET TRANSFORMED AXIS VECTOR (PLANE NORMAL)
				XMVECTOR planeNormal = XMVector3Normalize(XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), modelRotationMatrix));
				//GET INTERSECT POINT WITH THIS PLANE AND THE MOUSE RAY
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector(); //FIND VECTOR DIFFERENCE
				float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));//GET SCALAR DISTANCE
				if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f) { //CHECK IF THE INTERSECT IS ON THE RING
					XMVECTOR camToIntersect = planeIntersectPoint - camera.GetPositionVector(); //FIND VECTOR DIFF FROM CAMERA TO THE POINT
					float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect)); //FIND SCALAR DISTANCE FROM CAMERA TO THE POINT
					if (camToIntersectDist < closestDist) { //IF LESS THAN THE LAST DISTANCE SET VARIABLES
						closestDist = camToIntersectDist;
						this->axisEditSubState = AxisEditSubState::EDIT_X;
					}
				}
			}
			{
				XMVECTOR planeNormal = XMVector3Normalize(XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), modelRotationMatrix));
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector();
				float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
				if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f) {
					XMVECTOR camToIntersect = planeIntersectPoint - camera.GetPositionVector();
					float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect));
					if (camToIntersectDist < closestDist) {
						closestDist = camToIntersectDist;
						this->axisEditSubState = AxisEditSubState::EDIT_Y;
					}
				}
			}
			{
				XMVECTOR planeNormal = XMVector3Normalize(XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), modelRotationMatrix));
				XMVECTOR planeIntersectPoint = RayPlaneIntersect(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection(), planeNormal, XMVectorSet(objectPos.x, objectPos.y, objectPos.z, 0.0f));
				XMVECTOR centreDiff = planeIntersectPoint - this->selectedObject->GetPositionVector();
				float distanceToCentre = XMVectorGetX(XMVector3Length(centreDiff));
				if (distanceToCentre > objectHitRadius - objectHitRadius * 0.1f && distanceToCentre < objectHitRadius + objectHitRadius * 0.1f) {
					XMVECTOR camToIntersect = planeIntersectPoint - camera.GetPositionVector();
					float camToIntersectDist = XMVectorGetX(XMVector3Length(camToIntersect));
					if (camToIntersectDist < closestDist) {
						closestDist = camToIntersectDist;
						this->axisEditSubState = AxisEditSubState::EDIT_Z;
					}
				}
			}
			if (closestDist != FLT_MAX) {
				this->selectedObject->SetFollowingObjectTrack(false);
				return;
			}
		}
	}

	//MAN
	distance = this->nano.GetRayIntersectDist(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection());
	if (distance < closestDist) {
		closestDist = distance;
		this->selectedObject = &nano;
	}

	//LADY
	distance = this->woman.GetRayIntersectDist(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection());
	if (distance < closestDist) {
		closestDist = distance;
		this->selectedObject = &woman;
	}

	//POINT LIGHT
	distance = this->pointLight.GetRayIntersectDist(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection());
	if (distance < closestDist) {
		closestDist = distance;
		this->selectedObject = &pointLight;
	}

	//SPOT LIGIHT
	distance = this->spotLight.GetRayIntersectDist(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection());
	if (distance < closestDist) {
		closestDist = distance;
		this->selectedObject = &spotLight;
	}

	//BOAT
	distance = this->boat.GetRayIntersectDist(camera.GetPositionVector(), camera.GetMouseToWorldVectorDirection());
	if (distance < closestDist) {
		closestDist = distance;
		this->selectedObject = &boat;
	}

	//IF AN OBJECT WAS SELECTED, SET SELECTING TO TRUE
	if (closestDist != FLT_MAX) {
		this->selectingGameObject = true;
	}
	//OTHERWISE, DESELECT THE SELECTED OBJECT
	else {
		this->selectingGameObject = false;
		this->selectedObject = nullptr;
		this->axisEditSubState = AxisEditSubState::EDIT_NONE;
		this->lastAxisGrabOffset = FLT_MAX;
	}
}

void Graphics::DrawAxisForObject(GameObject* gameObject, const XMMATRIX& viewProjection)
{
	XMFLOAT3 gameObjectPosition = gameObject->GetPositionFloat3();
	XMMATRIX modelMatrix = XMMatrixTranslation(gameObjectPosition.x, gameObjectPosition.y, gameObjectPosition.z);

	float scale = this->selectedObject->GetModel()->GetHitRadius();

	//CLAMP SCALE
	if (scale < 0.75f) scale = 0.75f;

	if (this->axisEditState == AxisEditState::EDIT_TRANSLATE) {
		this->axisTranslateModel.Draw(XMMatrixScaling(scale, scale, scale) * modelMatrix, viewProjection);
	}
	else if (this->axisEditState == AxisEditState::EDIT_ROTATE) { //MULTIPLY BY ACTUAL MODEL MATRIX WHEN ROTATING
		this->axisRotateModel.Draw(XMMatrixScaling(scale, scale, scale) * this->selectedObject->GetModelMatrix(), viewProjection);
	}
}

AxisEditSubState Graphics::GetAxisEditSubState()
{
	return this->axisEditSubState;
}

void Graphics::StopAxisEdit()
{
	this->axisEditSubState = AxisEditSubState::EDIT_NONE;
	this->lastAxisGrabOffset = FLT_MAX;
}

XMVECTOR Graphics::RayPlaneIntersect(XMVECTOR rayPoint, XMVECTOR rayDirection, XMVECTOR planeNormal, XMVECTOR planePoint) {
	XMVECTOR diff = rayPoint - planePoint;
	return (diff + planePoint) + rayDirection * (-XMVector3Dot(diff, planeNormal) / XMVector3Dot(rayDirection, planeNormal));
}