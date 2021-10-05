#pragma once

#include "ParticleModel.h"
#include "../graphics/Model.h"

#define MAX_PARTICLE_COUNT 400

class ParticleSystemParticle
{
private:
	bool active = false;

	float currentAge = 0.0f;
	float maxAge = 0.0f;

	ParticleModel particleModel;
	Transform transform;

public:
	ParticleSystemParticle();
	void Update(float deltaTime);
	void Spawn(XMVECTOR position, XMVECTOR velocity, float maxAge);
	Transform* GetTransform();
	bool GetActive();
	float GetAge();
	float GetMaxAge();
};

class ParticleSystem; //Forward declare

class ParticleSystemEmitter
{
private:
	bool usingExternalPosition;
	XMVECTOR* externalPosition;

	XMVECTOR position;

	ParticleSystem* particleSystem;

	XMVECTOR direction;
	float directionRandomness; // 0 = Just uses specified direction, 1 = completely random direction

	float power;
	float powerRandomModifier;

	float maxAge;
	float maxAgeRandomModifier;

	float spawnDelay;
	float spawnDelayRandomModifier;

	float spawnTimer;

public:
	ParticleSystemEmitter(ParticleSystem* particleSystem, const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier);
	void Update(float deltaTime);
	void SpawnParticle();

	XMVECTOR* GetPosition();
	XMVECTOR* GetDirection();
	float* GetDirectionRandomnessPtr();

	float* GetPowerPtr();
	float* GetPowerRandomModifierPtr();

	float* GetMaxAgePtr();
	float* GetMaxAgeRandomModifierPtr();

	float* GetSpawnDelayPtr();
	float* GetSpawnDelayRandomModifierPtr();
};

class ParticleSystem
{
private:
	std::vector<ParticleSystemEmitter*> particleEmitters;
	ParticleSystemParticle particles[MAX_PARTICLE_COUNT];
	Model* particleMesh;

public:
	ParticleSystem(Model* particleMesh);
	void Update(float deltaTime);
	void DrawParticles(XMMATRIX viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader);
	void AddParticle(const XMVECTOR& position, const XMVECTOR& velocity, float maxAge);
	void AddEmitter(const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier);
	int FindNextAvailableParticleIndex();
	std::vector<ParticleSystemEmitter*>* GetEmitters();
};

