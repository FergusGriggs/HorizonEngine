#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Model* particleMesh)
{
	this->particleMesh = particleMesh;
}

void ParticleSystem::Update(float deltaTime)
{
	for (int i = 0; i < particleEmitters.size(); ++i)
	{
		this->particleEmitters[i]->Update(deltaTime);
	}

	for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
	{
		this->particles[i].Update(deltaTime);
	}
}

void ParticleSystem::DrawParticles(XMMATRIX viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader)
{
	XMMATRIX modelMatrix;
	XMFLOAT3 particlePosition;

	for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
	{
		if (particles[i].GetActive())
		{
			float particleAge = particles[i].GetAge();
			float particleMaxAge = particles[i].GetMaxAge();
			float particleScale = 0.25f;

			if (particleAge < particleMaxAge * 0.15f)
			{
				particleScale *= particleAge / (particleMaxAge * 0.15f);
			}
			else if (particleAge > particleMaxAge * 0.85f)
			{
				particleScale *= 1.0f - ((particleAge - particleMaxAge * 0.85f) / (particleMaxAge * 0.15f));
			}

			XMStoreFloat3(&particlePosition, this->particles[i].GetTransform()->GetPositionVector());
			modelMatrix = XMMatrixScaling(particleScale, particleScale, particleScale) * XMMatrixTranslation(particlePosition.x, particlePosition.y, particlePosition.z);
			particleMesh->Draw(modelMatrix, viewProjectionMatrix, cb_vs_vertexShader);
		}
	}
}

void ParticleSystem::AddParticle(const XMVECTOR& position, const XMVECTOR& velocity, float maxAge)
{
	int nextIndex = FindNextAvailableParticleIndex();

	if (nextIndex != -1)
	{
		this->particles[nextIndex].Spawn(position, velocity, maxAge);
	}
}

void ParticleSystem::AddEmitter(const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier)
{
	particleEmitters.push_back(new ParticleSystemEmitter(this, position, direction, directionRandomness, power, powerRandomModifier, maxAge, maxAgeRandomModifier, spawnDelay, spawnDelayRandomModifier));
}

int ParticleSystem::FindNextAvailableParticleIndex()
{
	for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
	{
		if (!particles[i].GetActive())
		{
			return i;
		}
	}

	return -1;
}

std::vector<ParticleSystemEmitter*>* ParticleSystem::GetEmitters()
{
	return &this->particleEmitters;
}

ParticleSystemEmitter::ParticleSystemEmitter(ParticleSystem* particleSystem, const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier)
{
	this->particleSystem = particleSystem;

	this->position = position;

	this->direction = direction;
	this->directionRandomness = directionRandomness;
	
	this->power = power;
	this->powerRandomModifier = powerRandomModifier;

	this->maxAge = maxAge;
	this->maxAgeRandomModifier = maxAgeRandomModifier;

	this->spawnDelay = spawnDelay;
	this->spawnDelayRandomModifier = spawnDelayRandomModifier;

	this->spawnTimer = 0.0f;
}

void ParticleSystemEmitter::Update(float deltaTime)
{
	this->spawnTimer -= deltaTime;

	if (spawnTimer < 0.0f)
	{
		this->spawnTimer += this->spawnDelay * (1.0f + this->spawnDelayRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

		this->SpawnParticle();
	}
}

void ParticleSystemEmitter::SpawnParticle()
{
	XMVECTOR newParticlePosition;
	if (usingExternalPosition)
	{
		newParticlePosition = *externalPosition;
	}
	else
	{
		newParticlePosition = this->position;
	}

	float newParticlePower = this->power * (1.0f + this->powerRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

	XMVECTOR randomVector = XMVectorSet(static_cast<float>(rand() % 200 - 100) * 0.01f, static_cast<float>(rand() % 200 - 100) * 0.01f, static_cast<float>(rand() % 200 - 100) * 0.01f, 0.0f);

	XMVECTOR newParticleVelocity = newParticlePower * XMVector3Normalize(XMVectorLerp(this->direction, randomVector, this->directionRandomness));

	float newParticleMaxAge = this->maxAge * (1.0f + this->maxAgeRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

	this->particleSystem->AddParticle(newParticlePosition, newParticleVelocity, newParticleMaxAge);
}

XMVECTOR* ParticleSystemEmitter::GetPosition()
{
	return &this->position;
}

XMVECTOR* ParticleSystemEmitter::GetDirection()
{
	return &this->direction;
}

float* ParticleSystemEmitter::GetDirectionRandomnessPtr()
{
	return &this->directionRandomness;
}

float* ParticleSystemEmitter::GetPowerPtr()
{
	return &this->power;
}

float* ParticleSystemEmitter::GetPowerRandomModifierPtr()
{
	return &this->powerRandomModifier;
}

float* ParticleSystemEmitter::GetMaxAgePtr()
{
	return &this->maxAge;
}

float* ParticleSystemEmitter::GetMaxAgeRandomModifierPtr()
{
	return &this->maxAgeRandomModifier;
}

float* ParticleSystemEmitter::GetSpawnDelayPtr()
{
	return &this->spawnDelay;
}

float* ParticleSystemEmitter::GetSpawnDelayRandomModifierPtr()
{
	return &this->spawnDelayRandomModifier;
}

ParticleSystemParticle::ParticleSystemParticle()
{
	this->particleModel.SetTransformReference(&this->transform);
}

void ParticleSystemParticle::Update(float deltaTime)
{
	if (this->active)
	{
		this->particleModel.Update(deltaTime);

		this->currentAge += deltaTime;

		if (this->currentAge > this->maxAge)
		{
			this->active = false;
		}
	}
}

void ParticleSystemParticle::Spawn(XMVECTOR position, XMVECTOR velocity, float maxAge)
{
	this->transform.SetPosition(position);
	this->particleModel.SetVelocity(velocity);
	this->maxAge = maxAge;
	this->currentAge = 0.0f;
	this->active = true;
	this->particleModel.SetIsStatic(false);
}

Transform* ParticleSystemParticle::GetTransform()
{
	return &this->transform;
}

bool ParticleSystemParticle::GetActive()
{
	return this->active;
}

float ParticleSystemParticle::GetAge()
{
	return this->currentAge;
}

float ParticleSystemParticle::GetMaxAge()
{
	return this->maxAge;
}
