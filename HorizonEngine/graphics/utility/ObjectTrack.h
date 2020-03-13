//ObjectTrack.h
//Stores information about a spline which a GameObject can follow smoothly

#pragma once

#include <iostream>
#include <vector>
#include <DirectXMath.h>

class GameObject;

using namespace DirectX;

struct ObjectTrackNode {
	XMFLOAT3 position;
	XMFLOAT3 lookPoint;

	ObjectTrackNode(XMFLOAT3 position, XMFLOAT3 lookPoint);
};

class ObjectTrack
{
private:
	std::vector<ObjectTrackNode> trackNodes;
	std::vector<ObjectTrackNode> midPoints;
	float trackSpeed;
	float maxDelta;
	bool loopTrack;
	float delta;
	std::string label = "null";

public:
	ObjectTrack();
	void AddTrackNode(ObjectTrackNode trackNode);
	void GenerateMidPoints();

	static float LerpFloat(float start, float end, float delta);
	static XMFLOAT3 LerpFloat3(XMFLOAT3 start, XMFLOAT3 end, float delta);
	static XMFLOAT3 LerpQuadratic(XMFLOAT3 start, XMFLOAT3 control, XMFLOAT3 end, float delta);

	float GetDelta();
	void SetDelta(float delta);

	std::string GetLabel();
	void SetLabel(std::string label);

	std::vector<ObjectTrackNode>* GetTrackNodes();

	void Follow(GameObject* gameObject, float deltaTime, bool lookTo = true);
};

#include "../GameObject.h"