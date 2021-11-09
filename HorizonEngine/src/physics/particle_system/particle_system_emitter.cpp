#include "particle_system_emitter.h"

#include "particle_system.h"

namespace hrzn::physics
{
	ParticleSystemEmitter::ParticleSystemEmitter(ParticleSystem* particleSystem, const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier) :
		m_particleSystem(particleSystem),

		m_active(false),

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
		if (m_active)
		{
			m_spawnTimer -= deltaTime;

			if (m_spawnTimer < 0.0f)
			{
				m_spawnTimer += m_spawnDelay * (1.0f + m_spawnDelayRandomModifier * static_cast<float>(rand() % 200 - 100) * 0.01f);

				spawnParticle();
			}
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

	bool* ParticleSystemEmitter::getActiveWritablePtr()
	{
		return &m_active;
	}

	XMVECTOR* ParticleSystemEmitter::getPositionWritablePtr()
	{
		return &m_position;
	}

	XMVECTOR* ParticleSystemEmitter::getDirectionWritablePtr()
	{
		return &m_direction;
	}

	float* ParticleSystemEmitter::getDirectionRandomnessWritablePtr()
	{
		return &m_directionRandomness;
	}

	float* ParticleSystemEmitter::getPowerWritablePtr()
	{
		return &m_power;
	}

	float* ParticleSystemEmitter::getPowerRandomModifierWritablePtr()
	{
		return &m_powerRandomModifier;
	}

	float* ParticleSystemEmitter::getMaxAgeWritablePtr()
	{
		return &m_maxAge;
	}

	float* ParticleSystemEmitter::getMaxAgeRandomModifierWritablePtr()
	{
		return &m_maxAgeRandomModifier;
	}

	float* ParticleSystemEmitter::getSpawnDelayWritablePtr()
	{
		return &m_spawnDelay;
	}

	float* ParticleSystemEmitter::getSpawnDelayRandomModifierWritablePtr()
	{
		return &m_spawnDelayRandomModifier;
	}
}
