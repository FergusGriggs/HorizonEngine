
#pragma once

#include "../particle_model.h"

namespace hrzn::physics
{
	class ParticleSystemParticle
	{
	public:
		ParticleSystemParticle();

		void update(float deltaTime);
		void spawn(XMVECTOR position, XMVECTOR velocity, float maxAge);

		const Transform& getTransform() const;
		Transform&       getWritableTransform();

		bool  getActive() const;
		float getAge() const;
		float getMaxAge() const;

	private:
		ParticleModel m_particleModel;
		Transform     m_transform;
		bool          m_active;

		float         m_currentAge;
		float         m_maxAge;
	};
}
