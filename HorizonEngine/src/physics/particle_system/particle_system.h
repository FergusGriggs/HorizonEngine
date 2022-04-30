#pragma once

#include "../particle_model.h"

#include "../../graphics/data/model.h"

#include "particle_system_emitter.h"

namespace hrzn::physics
{
	#define MAX_PARTICLE_COUNT 400

	class ParticleSystem
	{
	public:
		ParticleSystem();

		void update(float deltaTime);
		void drawParticles(bool bindPSData) const;

		void addParticle(const XMVECTOR& position, const XMVECTOR& velocity, float maxAge);
		void addEmitter(const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier);
		int  findNextAvailableParticleIndex() const;

		const std::vector<ParticleSystemEmitter*>& getEmitters();

	private:
		std::vector<ParticleSystemEmitter*> m_particleEmitters;
		ParticleSystemParticle              m_particles[MAX_PARTICLE_COUNT];

		gfx::Model* m_particleMesh;
	};
}
