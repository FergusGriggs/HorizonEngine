#include "Spring.h"

namespace hrzn::scene::entity::physics
{
	Spring::Spring(XMVECTOR startPosition, XMVECTOR endPosition, float restLength, float springConstant) :
		m_startIsReference(false),
		m_springStart(new ParticleModel()),

		m_endIsReference(false),
		m_springEnd(new ParticleModel()),
		
		m_restLength(restLength),
		m_springConstant(springConstant)
	{
		m_springStart->setTransformReference(new Transform(startPosition, XMQuaternionIdentity()));
		m_springEnd->setTransformReference(new Transform(endPosition, XMQuaternionIdentity()));
	}

	Spring::Spring(XMVECTOR startPosition, ParticleModel* endParticleModel, float restLength, float springConstant) :
		m_startIsReference(false),
		m_springStart(new ParticleModel()),

		m_endIsReference(true),
		m_springEnd(endParticleModel),

		m_restLength(restLength),
		m_springConstant(springConstant)
	{
		m_springStart->setTransformReference(new Transform(startPosition, XMQuaternionIdentity()));
	}

	Spring::Spring(ParticleModel* startParticleModel, XMVECTOR endPosition, float restLength, float springConstant) :
		m_startIsReference(true),
		m_springStart(startParticleModel),

		m_endIsReference(false),
		m_springEnd(new ParticleModel()),

		m_restLength(restLength),
		m_springConstant(springConstant)
	{
		m_springEnd->setTransformReference(new Transform(endPosition, XMQuaternionIdentity()));
	}

	Spring::Spring(ParticleModel* startParticleModel, ParticleModel* endParticleModel, float restLength, float springConstant) :
		m_startIsReference(true),
		m_springStart(startParticleModel),

		m_endIsReference(true),
		m_springEnd(endParticleModel),

		m_restLength(restLength),
		m_springConstant(springConstant)
	{
	}

	Spring::~Spring()
	{
		if (!m_startIsReference)
		{
			delete m_springStart->getTransformReference();
			delete m_springStart;
		}
		if (!m_endIsReference)
		{
			delete m_springEnd->getTransformReference();
			delete m_springEnd;
		}
	}

	ParticleModel* Spring::getSpringStart()
	{
		return m_springStart;
	}

	ParticleModel* Spring::getSpringEnd()
	{
		return m_springEnd;
	}

	void Spring::update()
	{
		XMVECTOR difference = m_springEnd->getTransformReference()->getPositionVector() - m_springStart->getTransformReference()->getPositionVector();
		float forceMagnitude = m_springConstant * (XMVectorGetX(XMVector3Length(difference)) - m_restLength); //F = Kx

		XMVECTOR force = XMVector3Normalize(difference) * forceMagnitude * 0.5f;

		m_springStart->addForce(force);
		m_springEnd->addForce(-force);
	}
}
