#include "particle_system_particle.h"

namespace hrzn::physics
{
	ParticleSystemParticle::ParticleSystemParticle()
	{
		m_particleModel.setTransformReference(&m_transform);
	}

	void ParticleSystemParticle::update(float deltaTime)
	{
		if (m_active)
		{
			m_particleModel.update(deltaTime);

			m_currentAge += deltaTime;

			if (m_currentAge > m_maxAge)
			{
				m_active = false;
			}
		}
	}

	void ParticleSystemParticle::spawn(XMVECTOR position, XMVECTOR velocity, float maxAge)
	{
		m_transform.setPosition(position);
		m_particleModel.setVelocity(velocity);
		m_maxAge = maxAge;
		m_currentAge = 0.0f;
		m_active = true;
		m_particleModel.setIsStatic(false);
	}

	const Transform& ParticleSystemParticle::getTransform() const
	{
		return m_transform;
	}

	Transform& ParticleSystemParticle::getWritableTransform()
	{
		return m_transform;
	}

	bool ParticleSystemParticle::getActive() const
	{
		return m_active;
	}

	float ParticleSystemParticle::getAge() const
	{
		return m_currentAge;
	}

	float ParticleSystemParticle::getMaxAge() const
	{
		return m_maxAge;
	}
}