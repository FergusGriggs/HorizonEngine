#pragma once

#include "particle_model.h"

namespace hrzn::entity::physics
{
	class Spring
	{
	public:
		Spring(XMVECTOR startPosition, XMVECTOR endPosition, float restLength, float springConstant);
		Spring(XMVECTOR startPosition, ParticleModel* endParticleModel, float restLength, float springConstant);
		Spring(ParticleModel* startParticleModel, XMVECTOR endPosition, float restLength, float springConstant);
		Spring(ParticleModel* startParticleModel, ParticleModel* endParticleModel, float restLength, float springConstant);
		~Spring();

		ParticleModel* getSpringStart();
		ParticleModel* getSpringEnd();

		void update();

	private:
		bool           m_startIsReference;
		ParticleModel* m_springStart;

		bool           m_endIsReference;
		ParticleModel* m_springEnd;

		float          m_restLength;
		float          m_springConstant;
	};
}
