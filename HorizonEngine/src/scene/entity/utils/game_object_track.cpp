
//Function implementations for the ObjectTrack class

#include "game_object_track.h"

#include "../game_object.h"

namespace hrzn::scene::entity
{
	GameObjectTrack::GameObjectTrack() :
		m_label("null"),
		m_trackNodes(),
		m_midPoints(),

		m_delta(0.0f),
		m_maxDelta(0.0f),

		m_trackSpeed(0.5f),
		m_loopTrack(true)
	{
	}

	void GameObjectTrack::addTrackNode(ObjectTrackNode trackNode)
	{
		m_trackNodes.push_back(trackNode);
	}

	void GameObjectTrack::generateMidPoints()
	{
		m_midPoints.clear();
		for (int i = 0; i < m_trackNodes.size() - 1; i++)
		{
			XMFLOAT3 position = lerpFloat3(m_trackNodes[i].m_position, m_trackNodes[i + 1].m_position, 0.5f);
			XMFLOAT3 lookPoint = lerpFloat3(m_trackNodes[i].m_lookPoint, m_trackNodes[i + 1].m_lookPoint, 0.5f);
			m_midPoints.push_back(ObjectTrackNode(position, lookPoint));
		}
		m_maxDelta = static_cast<float>(m_trackNodes.size() - 1);
	}

	float GameObjectTrack::lerpFloat(float start, float end, float delta)
	{
		return start + delta * (end - start);
	}

	XMFLOAT3 GameObjectTrack::lerpFloat3(XMFLOAT3 start, XMFLOAT3 end, float delta)
	{
		return XMFLOAT3(lerpFloat(start.x, end.x, delta), lerpFloat(start.y, end.y, delta), lerpFloat(start.z, end.z, delta));
	}

	XMFLOAT3 GameObjectTrack::lerpQuadratic(XMFLOAT3 start, XMFLOAT3 control, XMFLOAT3 end, float delta)
	{
		return lerpFloat3(lerpFloat3(start, control, delta), lerpFloat3(control, end, delta), delta);
	}

	void GameObjectTrack::follow(GameObject* gameObject, float deltaTime, bool lookTo)
	{
		m_delta += m_trackSpeed * deltaTime * 0.001f;

		if (m_delta < 0.0f)
		{
			m_delta = m_maxDelta;
		}
		else if (m_delta > m_maxDelta)
		{
			m_delta = 0.0f;
		}
		int trackSize = m_trackNodes.size();
		if (trackSize == 0)
		{
			gameObject->getTransform().setPosition(m_trackNodes[0].m_position);
			if (lookTo) gameObject->getTransform().lookAtPosition(m_trackNodes[0].m_lookPoint);
		}
		else if (trackSize == 2)
		{
			gameObject->getTransform().setPosition(lerpFloat3(m_trackNodes[0].m_position, m_trackNodes[1].m_position, m_delta));
			if (lookTo) gameObject->getTransform().lookAtPosition(lerpFloat3(m_trackNodes[0].m_lookPoint, m_trackNodes[1].m_lookPoint, m_delta));
		}
		else if (trackSize >= 3)
		{
			float upperDelta = (float)m_trackNodes.size() - 1.5f;
			if (m_loopTrack)
			{
				if (m_delta < 0.5f)
				{
					gameObject->getTransform().setPosition(lerpQuadratic(m_trackNodes[trackSize - 2].m_position, m_trackNodes[0].m_position, m_trackNodes[0].m_position, 0.5f + m_delta));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpQuadratic(m_midPoints[trackSize - 2].m_lookPoint, m_trackNodes[0].m_lookPoint, m_midPoints[0].m_lookPoint, 0.5f + m_delta));
				}
				else if (m_delta < upperDelta)
				{
					int splineIndex = floor(m_delta - 0.5f);
					float splineDelta = m_delta - 0.5f - (float)splineIndex;
					gameObject->getTransform().setPosition(lerpQuadratic(m_trackNodes[splineIndex].m_position, m_trackNodes[splineIndex + 1].m_position, m_trackNodes[splineIndex + 1].m_position, splineDelta));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpQuadratic(m_midPoints[splineIndex].m_lookPoint, m_trackNodes[splineIndex + 1].m_lookPoint, m_midPoints[splineIndex + 1].m_lookPoint, splineDelta));
				}
				else if (m_delta > upperDelta)
				{
					float splineDelta = m_delta - upperDelta;
					gameObject->getTransform().setPosition(lerpQuadratic(m_trackNodes[trackSize - 2].m_position, m_trackNodes[0].m_position, m_trackNodes[0].m_position, splineDelta));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpQuadratic(m_midPoints[trackSize - 2].m_lookPoint, m_trackNodes[0].m_lookPoint, m_midPoints[0].m_lookPoint, splineDelta));
				}
			}
			else
			{
				if (m_delta < 0.5f)
				{
					gameObject->getTransform().setPosition(lerpFloat3(m_trackNodes[0].m_position, m_midPoints[0].m_position, m_delta * 2.0f));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpFloat3(m_trackNodes[0].m_lookPoint, m_midPoints[0].m_lookPoint, m_delta * 2.0f));
				}
				else if (m_delta < upperDelta)
				{
					int splineIndex = floor(m_delta - 0.5f);
					float splineDelta = m_delta - 0.5f - (float)splineIndex;
					gameObject->getTransform().setPosition(lerpQuadratic(m_midPoints[splineIndex].m_position, m_trackNodes[splineIndex + 1].m_position, m_midPoints[splineIndex + 1].m_position, splineDelta));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpQuadratic(m_midPoints[splineIndex].m_lookPoint, m_trackNodes[splineIndex + 1].m_lookPoint, m_midPoints[splineIndex + 1].m_lookPoint, splineDelta));
				}
				else if (m_delta > upperDelta)
				{
					int splineIndex = floor(m_delta - 0.5f);
					float splineDelta = m_delta - (float)splineIndex;
					gameObject->getTransform().setPosition(lerpFloat3(m_midPoints[splineIndex].m_position, m_trackNodes[splineIndex + 1].m_position, (splineDelta - 0.5f) * 2.0f));
					if (lookTo) gameObject->getTransform().lookAtPosition(lerpFloat3(m_midPoints[splineIndex].m_lookPoint, m_trackNodes[splineIndex + 1].m_lookPoint, (splineDelta - 0.5f) * 2.0f));
				}
			}
		}
	}

	float GameObjectTrack::getDelta()
	{
		return m_delta;
	}

	void GameObjectTrack::setDelta(float delta)
	{
		delta = delta;
	}

	std::string GameObjectTrack::getLabel()
	{
		return m_label;
	}

	void GameObjectTrack::setLabel(std::string label)
	{
		label = label;
	}

	std::vector<ObjectTrackNode>* GameObjectTrack::getTrackNodes()
	{
		return &m_trackNodes;
	}
}
