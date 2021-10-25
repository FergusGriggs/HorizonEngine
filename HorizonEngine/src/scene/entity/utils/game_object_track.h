
//Stores information about a spline which a GameObject can follow smoothly

#pragma once

#include <iostream>
#include <vector>

#include <DirectXMath.h>

// forward declares
namespace hrzn::scene::entity
{
	class GameObject;
}

using namespace DirectX;

namespace hrzn::scene::entity
{
	struct ObjectTrackNode
	{
		ObjectTrackNode(XMFLOAT3 position, XMFLOAT3 lookPoint) :
			m_position(position),
			m_lookPoint(lookPoint)
		{
		}

		XMFLOAT3 m_position;
		XMFLOAT3 m_lookPoint;
	};

	class GameObjectTrack
	{
	public:
		GameObjectTrack();

		void addTrackNode(ObjectTrackNode trackNode);
		void generateMidPoints();

		static float lerpFloat(float start, float end, float delta);
		static XMFLOAT3 lerpFloat3(XMFLOAT3 start, XMFLOAT3 end, float delta);
		static XMFLOAT3 lerpQuadratic(XMFLOAT3 start, XMFLOAT3 control, XMFLOAT3 end, float delta);

		float getDelta();
		void setDelta(float delta);

		std::string getLabel();
		void setLabel(std::string label);

		std::vector<ObjectTrackNode>* getTrackNodes();

		void follow(GameObject* gameObject, float deltaTime, bool lookTo = true);

	private:
		std::string m_label;

		std::vector<ObjectTrackNode> m_trackNodes;
		std::vector<ObjectTrackNode> m_midPoints;

		float m_delta;
		float m_maxDelta;

		float m_trackSpeed;
		bool  m_loopTrack;
	};
}