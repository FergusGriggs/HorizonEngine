
#pragma once

#include "../particle_model.h"

namespace hrzn::entity::physics
{
	class ParticleSystemParticle
	{
	public:
		ParticleSystemParticle();

		void update(float deltaTime);
		void spawn(XMVECTOR position, XMVECTOR velocity, float maxAge);

		Transform& getTransform();
		bool       getActive();
		float      getAge();
		float      getMaxAge();

	private:
		ParticleModel m_particleModel;
		Transform     m_transform;
		bool          m_active;

		float         m_currentAge;
		float         m_maxAge;
	};
}
