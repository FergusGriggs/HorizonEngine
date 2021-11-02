
#pragma once

// forward declares
namespace hrzn::physics
{
	class ParticleSystem;
}

#include "particle_system_particle.h"

namespace hrzn::physics
{
	class ParticleSystemEmitter
	{
	public:
		ParticleSystemEmitter(ParticleSystem* particleSystem, const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier);

		void update(float deltaTime);
		void spawnParticle();

		bool*     getActiveWritablePtr();

		XMVECTOR* getPositionWritablePtr();

		XMVECTOR* getDirectionWritablePtr();
		float*    getDirectionRandomnessWritablePtr();
			      
		float*    getPowerWritablePtr();
		float*    getPowerRandomModifierWritablePtr();
			      
		float*    getMaxAgeWritablePtr();
		float*    getMaxAgeRandomModifierWritablePtr();
			      
		float*    getSpawnDelayWritablePtr();
		float*    getSpawnDelayRandomModifierWritablePtr();

	private:
		ParticleSystem* m_particleSystem;

		bool      m_active;

		XMVECTOR  m_position;

		XMVECTOR  m_direction;
		float     m_directionRandomness; // 0 = Just uses specified direction, 1 = completely random direction

		bool      m_usingExternalPosition;
		XMVECTOR* m_externalPosition;

		float     m_power;
		float     m_powerRandomModifier;

		float     m_maxAge;
		float     m_maxAgeRandomModifier;

		float     m_spawnDelay;
		float     m_spawnDelayRandomModifier;

		float     m_spawnTimer;
	};
}