#include "particle_system.h"

namespace hrzn::entity::physics
{
	ParticleSystem::ParticleSystem(gfx::Model* particleMesh) :
		m_particleEmitters(),
		m_particles(),
		m_particleMesh(particleMesh)
	{
	}

	void ParticleSystem::update(float deltaTime)
	{
		for (int i = 0; i < m_particleEmitters.size(); ++i)
		{
			m_particleEmitters[i]->update(deltaTime);
		}

		for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
		{
			m_particles[i].update(deltaTime);
		}
	}

	void ParticleSystem::drawParticles(XMMATRIX viewProjectionMatrix, gfx::ConstantBuffer<gfx::VertexShaderCB>* vertexShaderCB)
	{
		XMMATRIX modelMatrix;
		XMFLOAT3 particlePosition;

		for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
		{
			if (m_particles[i].getActive())
			{
				float particleAge = m_particles[i].getAge();
				float particleMaxAge = m_particles[i].getMaxAge();
				float particleScale = 0.25f;

				if (particleAge < particleMaxAge * 0.15f)
				{
					particleScale *= particleAge / (particleMaxAge * 0.15f);
				}
				else if (particleAge > particleMaxAge * 0.85f)
				{
					particleScale *= 1.0f - ((particleAge - particleMaxAge * 0.85f) / (particleMaxAge * 0.15f));
				}

				XMStoreFloat3(&particlePosition, m_particles[i].getTransform().getPositionVector());
				modelMatrix = XMMatrixScaling(particleScale, particleScale, particleScale) * XMMatrixTranslation(particlePosition.x, particlePosition.y, particlePosition.z);
				m_particleMesh->draw(modelMatrix, viewProjectionMatrix, vertexShaderCB);
			}
		}
	}

	void ParticleSystem::addParticle(const XMVECTOR& position, const XMVECTOR& velocity, float maxAge)
	{
		int nextIndex = findNextAvailableParticleIndex();

		if (nextIndex != -1)
		{
			m_particles[nextIndex].spawn(position, velocity, maxAge);
		}
	}

	void ParticleSystem::addEmitter(const XMVECTOR& position, const XMVECTOR& direction, float directionRandomness, float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier)
	{
		m_particleEmitters.push_back(new ParticleSystemEmitter(this, position, direction, directionRandomness, power, powerRandomModifier, maxAge, maxAgeRandomModifier, spawnDelay, spawnDelayRandomModifier));
	}

	int ParticleSystem::findNextAvailableParticleIndex()
	{
		for (int i = 0; i < MAX_PARTICLE_COUNT; ++i)
		{
			if (!m_particles[i].getActive())
			{
				return i;
			}
		}

		return -1;
	}

	std::vector<ParticleSystemEmitter*>* ParticleSystem::getEmitters()
	{
		return &m_particleEmitters;
	}
}
