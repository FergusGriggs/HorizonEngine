
//Stores information about a spline which a GameObject can follow smoothly

#pragma once

#include <iostream>
#include <vector>

#include <DirectXMath.h>

// forward declares
namespace hrzn::entity
{
	class GameObject;
}

using namespace DirectX;

namespace hrzn::entity
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
		static XMFLOAT3 lerpFloat3Quadratic(XMFLOAT3 start, XMFLOAT3 control, XMFLOAT3 end, float delta);

		float getDelta() const;
		void  setDelta(float delta);

		void  setSpeed(float speed);
		float getSpeed();

		const std::string& getId() const;
		void               setId(std::string label);

		const std::vector<ObjectTrackNode>& getTrackNodes() const;

		void follow(GameObject* gameObject, float deltaTime, bool lookTo = true);

	private:
		std::string m_id;

		std::vector<ObjectTrackNode> m_trackNodes;
		std::vector<ObjectTrackNode> m_midPoints;

		float m_delta;
		float m_maxDelta;

		float m_trackSpeed;
		bool  m_loopTrack;
	};
}