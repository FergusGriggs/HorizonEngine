#include "Spring.h"

Spring::Spring(XMVECTOR startPosition, XMVECTOR endPosition, float restLength, float springConstant)
{
	this->startIsReference = false;
	this->springStart = new ParticleModel();
	this->springStart->SetTransformReference(new Transform(startPosition, XMQuaternionIdentity()));

	this->endIsReference = false;
	this->springEnd = new ParticleModel();
	this->springEnd->SetTransformReference(new Transform(endPosition, XMQuaternionIdentity()));

	this->restLength = restLength;
	this->springConstant = springConstant;
}

Spring::Spring(XMVECTOR startPosition, ParticleModel* endParticleModel, float restLength, float springConstant)
{
	this->startIsReference = false;
	this->springStart = new ParticleModel();
	this->springStart->SetTransformReference(new Transform(startPosition, XMQuaternionIdentity()));

	this->endIsReference = true;
	this->springEnd = endParticleModel;

	this->restLength = restLength;
	this->springConstant = springConstant;
}

Spring::Spring(ParticleModel* startParticleModel, XMVECTOR endPosition, float restLength, float springConstant)
{
	this->startIsReference = true;
	this->springStart = startParticleModel;

	this->endIsReference = false;
	this->springEnd = new ParticleModel();
	this->springEnd->SetTransformReference(new Transform(endPosition, XMQuaternionIdentity()));

	this->restLength = restLength;
	this->springConstant = springConstant;
}

Spring::Spring(ParticleModel* startParticleModel, ParticleModel* endParticleModel, float restLength, float springConstant)
{
	this->startIsReference = true;
	this->springStart = startParticleModel;
	this->endIsReference = true;
	this->springEnd = endParticleModel;

	this->restLength = restLength;
	this->springConstant = springConstant;
}

Spring::~Spring()
{
	if (!startIsReference)
	{
		delete springStart->GetTransformReference();
		delete springStart;
	}
	if (!endIsReference)
	{
		delete springEnd->GetTransformReference();
		delete springEnd;
	}
}

ParticleModel* Spring::GetSpringStart()
{
	return this->springStart;
}

ParticleModel* Spring::GetSpringEnd()
{
	return this->springEnd;
}

void Spring::Update()
{
	XMVECTOR difference = springEnd->GetTransformReference()->GetPositionVector() - springStart->GetTransformReference()->GetPositionVector();
	float forceMagnitude = this->springConstant * (XMVectorGetX(XMVector3Length(difference)) - this->restLength); //F = Kx

	XMVECTOR force = XMVector3Normalize(difference) * forceMagnitude;

	springStart->AddForce(force);
	springEnd->AddForce(-force);
}
