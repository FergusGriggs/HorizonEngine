//ObjectTrack.cpp
//Function implementations for the ObjectTrack class

#include "ObjectTrack.h"

ObjectTrackNode::ObjectTrackNode(XMFLOAT3 position, XMFLOAT3 lookPoint)
{
	this->position = position;
	this->lookPoint = lookPoint;
}

ObjectTrack::ObjectTrack()
{
	this->maxDelta = 0.0f;
	this->trackSpeed = 0.5f;
	this->loopTrack = true;
}

void ObjectTrack::AddTrackNode(ObjectTrackNode trackNode)
{
	this->trackNodes.push_back(trackNode);
}

void ObjectTrack::GenerateMidPoints()
{
	midPoints.clear();
	for (int i = 0; i < this->trackNodes.size() - 1; i++) {
		XMFLOAT3 position = LerpFloat3(this->trackNodes.at(i).position, this->trackNodes.at(i + 1).position, 0.5f);
		XMFLOAT3 lookPoint = LerpFloat3(this->trackNodes.at(i).lookPoint, this->trackNodes.at(i + 1).lookPoint, 0.5f);
		midPoints.push_back(ObjectTrackNode(position, lookPoint));
	}
	maxDelta = static_cast<float>(this->trackNodes.size() - 1);
}

float ObjectTrack::LerpFloat(float start, float end, float delta)
{
	return start + delta * (end - start);
}

XMFLOAT3 ObjectTrack::LerpFloat3(XMFLOAT3 start, XMFLOAT3 end, float delta)
{
	return XMFLOAT3(LerpFloat(start.x, end.x, delta), LerpFloat(start.y, end.y, delta), LerpFloat(start.z, end.z, delta));
}

XMFLOAT3 ObjectTrack::LerpQuadratic(XMFLOAT3 start, XMFLOAT3 control, XMFLOAT3 end, float delta)
{
	return LerpFloat3(LerpFloat3(start, control, delta), LerpFloat3(control, end, delta), delta);
}

void ObjectTrack::Follow(GameObject* gameObject, float deltaTime, bool lookTo)
{
	this->delta += trackSpeed * deltaTime * 0.001f;

	if (this->delta < 0.0f) {
		this->delta = maxDelta;
	}
	else if (this->delta > maxDelta) {
		this->delta = 0.0f;
	}
	int trackSize = this->trackNodes.size();
	if (trackSize == 0) {
		gameObject->SetPosition(this->trackNodes[0].position);
		if (lookTo) gameObject->SetLookAtPos(this->trackNodes[0].lookPoint);
	}
	else if (trackSize == 2) {
		gameObject->SetPosition(LerpFloat3(this->trackNodes[0].position, this->trackNodes[1].position, this->delta));
		if (lookTo) gameObject->SetLookAtPos(LerpFloat3(this->trackNodes[0].lookPoint, this->trackNodes[1].lookPoint, this->delta));
	}
	else if (trackSize >= 3) {
		float upperDelta = (float)this->trackNodes.size() - 1.5f;
		if (loopTrack) {
			if (this->delta < 0.5f) {
				gameObject->SetPosition(LerpQuadratic(this->midPoints[trackSize - 2].position, this->trackNodes[0].position, this->midPoints[0].position, 0.5f + this->delta));
				if (lookTo) gameObject->SetLookAtPos(LerpQuadratic(this->midPoints[trackSize - 2].lookPoint, this->trackNodes[0].lookPoint, this->midPoints[0].lookPoint, 0.5f + this->delta));
			}
			else if (this->delta < upperDelta) {
				int splineIndex = floor(this->delta - 0.5f);
				float splineDelta = this->delta - 0.5f - (float)splineIndex;
				gameObject->SetPosition(LerpQuadratic(this->midPoints[splineIndex].position, this->trackNodes[splineIndex + 1].position, this->midPoints[splineIndex + 1].position, splineDelta));
				if (lookTo) gameObject->SetLookAtPos(LerpQuadratic(this->midPoints[splineIndex].lookPoint, this->trackNodes[splineIndex + 1].lookPoint, this->midPoints[splineIndex + 1].lookPoint, splineDelta));
			}
			else if (this->delta > upperDelta) {
				float splineDelta = this->delta - upperDelta;
				gameObject->SetPosition(LerpQuadratic(this->midPoints[trackSize - 2].position, this->trackNodes[0].position, this->midPoints[0].position, splineDelta));
				if (lookTo) gameObject->SetLookAtPos(LerpQuadratic(this->midPoints[trackSize - 2].lookPoint, this->trackNodes[0].lookPoint, this->midPoints[0].lookPoint, splineDelta));
			}
		}
		else {
			if (this->delta < 0.5f) {
				gameObject->SetPosition(LerpFloat3(this->trackNodes[0].position, this->midPoints[0].position, this->delta * 2.0f));
				if (lookTo) gameObject->SetLookAtPos(LerpFloat3(this->trackNodes[0].lookPoint, this->midPoints[0].lookPoint, this->delta * 2.0f));
			}
			else if (this->delta < upperDelta) {
				int splineIndex = floor(this->delta - 0.5f);
				float splineDelta = this->delta - 0.5f - (float)splineIndex;
				gameObject->SetPosition(LerpQuadratic(this->midPoints[splineIndex].position, this->trackNodes[splineIndex + 1].position, this->midPoints[splineIndex + 1].position, splineDelta));
				if (lookTo) gameObject->SetLookAtPos(LerpQuadratic(this->midPoints[splineIndex].lookPoint, this->trackNodes[splineIndex + 1].lookPoint, this->midPoints[splineIndex + 1].lookPoint, splineDelta));
			}
			else if (this->delta > upperDelta) {
				int splineIndex = floor(this->delta - 0.5f);
				float splineDelta = this->delta - (float)splineIndex;
				gameObject->SetPosition(LerpFloat3(this->midPoints[splineIndex].position, this->trackNodes[splineIndex + 1].position, (splineDelta - 0.5f) * 2.0f));
				if (lookTo) gameObject->SetLookAtPos(LerpFloat3(this->midPoints[splineIndex].lookPoint, this->trackNodes[splineIndex + 1].lookPoint, (splineDelta - 0.5f) * 2.0f));
			}
		}
	}
}

float ObjectTrack::GetDelta() {
	return this->delta;
}

void ObjectTrack::SetDelta(float delta) {
	this->delta = delta;
}
