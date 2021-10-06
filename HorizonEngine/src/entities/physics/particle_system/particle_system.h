#pragma once

#include "../particle_model.h"
#include "../../../graphics/data/model.h"

#include "particle_system_emitter.h"

namespace hrzn::entity::physics
{
	#define MAX_PARTICLE_COUNT 400

	class ParticleSystem
	{
	public:
		ParticleSystem(gfx::Model* particleMesh);

		void update(float deltaTime);
		void drawParticles(XMMATRIX viewProjectionMatrix, gfx::ConstantBuffer<gfx::CB_VS_vertexShader>* cb_vs_vertexShader);

		void addParticle(const XMVECTOR& position, const XMVECTOR& velocity, float maxAge);
		void addEmitter(const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier);
		int  findNextAvailableParticleIndex();

		std::vector<ParticleSystemEmitter*>* getEmitters();

	private:
		std::vector<ParticleSystemEmitter*> m_particleEmitters;
		ParticleSystemParticle              m_particles[MAX_PARTICLE_COUNT];

		gfx::Model* m_particleMesh;
	};
}
