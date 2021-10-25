#include "particle_system_emitter.h"

#include "particle_system.h"

namespace hrzn::scene::entity::physics
{
	ParticleSystemEmitter::ParticleSystemEmitter(ParticleSystem* particleSystem, const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier) :
		m_particleSystem(particleSystem),

		m_position(position),

		m_direction(direction),
		m_directionRandomness(directionRandomness), // 0 = Just uses specified direction, 1 = completely random direction

		m_usingExternalPosition(false),
		m_externalPosition(nullptr),

		m_power(power),
		m_powerRandomModifier(powerRandomModifier),

		m_maxAge(),
		m_maxAgeRandomModifier(),

		m_spawnDelay(),
		m_spawnDelayRandomModifier(),

		m_spawnTimer()
	{
	}

	void ParticleSystemEmitter::update(float deltaTime)
	{
		m_spawnTimer -= deltaTime;

		if (m_spawnTimer < 0.0f)
		{
			m_spawnTimer += m_spawnDelay * (1.0f + m_spawnDelayRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

			spawnParticle();
		}
	}

	void ParticleSystemEmitter::spawnParticle()
	{
		XMVECTOR newParticlePosition;
		if (m_usingExternalPosition)
		{
			newParticlePosition = *m_externalPosition;
		}
		else
		{
			newParticlePosition = m_position;
		}

		float newParticlePower = m_power * (1.0f + m_powerRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

		XMVECTOR randomVector = XMVectorSet(static_cast<float>(rand() % 200 - 100) * 0.01f, static_cast<float>(rand() % 200 - 100) * 0.01f, static_cast<float>(rand() % 200 - 100) * 0.01f, 0.0f);

		XMVECTOR newParticleVelocity = newParticlePower * XMVector3Normalize(XMVectorLerp(m_direction, randomVector, m_directionRandomness));

		float newParticleMaxAge = m_maxAge * (1.0f + m_maxAgeRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

		m_particleSystem->addParticle(newParticlePosition, newParticleVelocity, newParticleMaxAge);
	}

	XMVECTOR* ParticleSystemEmitter::getPosition()
	{
		return &m_position;
	}

	XMVECTOR* ParticleSystemEmitter::getDirection()
	{
		return &m_direction;
	}

	float* ParticleSystemEmitter::getDirectionRandomnessPtr()
	{
		return &m_directionRandomness;
	}

	float* ParticleSystemEmitter::getPowerPtr()
	{
		return &m_power;
	}

	float* ParticleSystemEmitter::getPowerRandomModifierPtr()
	{
		return &m_powerRandomModifier;
	}

	float* ParticleSystemEmitter::getMaxAgePtr()
	{
		return &m_maxAge;
	}

	float* ParticleSystemEmitter::getMaxAgeRandomModifierPtr()
	{
		return &m_maxAgeRandomModifier;
	}

	float* ParticleSystemEmitter::getSpawnDelayPtr()
	{
		return &m_spawnDelay;
	}

	float* ParticleSystemEmitter::getSpawnDelayRandomModifierPtr()
	{
		return &m_spawnDelayRandomModifier;
	}
}
