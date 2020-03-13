#pragma once

#include "RenderableGameObject.h"
#include "../physics/ParticleModel.h"

class PhysicsGameObject : public RenderableGameObject
{
public:
	PhysicsGameObject();
	bool Initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager);

	void SetMass(float mass);
	void SetDrag(float drag);

	float GetMass();
	float GetDrag();

	ParticleModel* GetParticleModel();

	void Update(float deltaTime) override;

private:
	ParticleModel particleModel;
};

