#pragma once

#include "ParticleModel.h"

class Spring
{
private:
	bool startIsReference;
	ParticleModel* springStart;

	bool endIsReference;
	ParticleModel* springEnd;

	float restLength;
	float springConstant;

public:
	Spring(XMVECTOR startPosition, XMVECTOR endPosition, float restLength, float springConstant);
	Spring(XMVECTOR startPosition, ParticleModel* endParticleModel, float restLength, float springConstant);
	Spring(ParticleModel* startParticleModel, XMVECTOR endPosition, float restLength, float springConstant);
	Spring(ParticleModel* startParticleModel, ParticleModel* endParticleModel, float restLength, float springConstant);
	~Spring();

	ParticleModel* GetSpringStart();
	ParticleModel* GetSpringEnd();

	void Update();
};

