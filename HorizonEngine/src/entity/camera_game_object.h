
//Primarily modifies view and projection matrices so that a scene can be rendered from its perspective

#pragma once

#include <DirectXMath.h>
#include "game_object.h"
#include "utils/game_object_track.h"

using namespace DirectX;

namespace hrzn::entity
{
	class CameraGameObject : public GameObject
	{
	public:
		CameraGameObject();

		void  update(float deltaTime) override;
		void  setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
		void  zoom(float fovDiff);
		void  setFOV(float fov);
		float getFOV() const;

		const XMMATRIX& getViewMatrix() const;
		const XMMATRIX& getProjectionMatrix() const;

		void            updateMouseToWorldVectorDirection();
		const XMVECTOR& getMouseToWorldVectorDirection() const;

		XMFLOAT2 getNDCFrom3DPos(XMVECTOR objectPosition) const;

		void     setRelativeObject(const GameObject* relativeObject, const XMVECTOR& relativePosition);
		void     setRelativeObject(const GameObject* relativeObject, const XMFLOAT3& relativePosition);
		void     unsetRelativeObject();

		bool    isUsingRelativeCamera();

		void     updateView();

	private:
		XMMATRIX m_view;
		XMMATRIX m_projection;

		float m_fov;
		float m_aspectRatio;
		float m_nearZ;
		float m_farZ;

		XMVECTOR m_mouseToWorldVectorDirection;

		const GameObject* m_relativeObject;
		XMVECTOR          m_relativePosition;
	};
}
