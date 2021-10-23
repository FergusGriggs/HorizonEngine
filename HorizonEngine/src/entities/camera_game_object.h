
//Primarily modifies view and projection matrices so that a scene can be rendered from its perspective

#pragma once

#include <DirectXMath.h>
#include "game_object.h"
#include "utility/game_object_track.h"

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
		float getFOV();

		const XMMATRIX& getViewMatrix() const;
		const XMMATRIX& getProjectionMatrix() const;

		void     computeMouseToWorldVectorDirection(float mouseNDCX, float mouseNDCY);
		XMVECTOR getMouseToWorldVectorDirection();

		XMFLOAT2 getNDCFrom3DPos(XMVECTOR objectPosition);

		void     setRelativeObject(GameObject* relativeObject, const XMVECTOR& relativePosition);
		void     setRelativeObject(GameObject* relativeObject, const XMFLOAT3& relativePosition);

		bool*    getUsingRelativeCameraPtr();

		void     updateView();

	private:
		XMMATRIX m_view;
		XMMATRIX m_projection;

		float m_fov;
		float m_aspectRatio;
		float m_nearZ;
		float m_farZ;

		XMVECTOR m_mouseToWorldVectorDirection;

		bool        m_usingRelativePosition;
		GameObject* m_relativeObject;
		XMVECTOR    m_relativePosition;
	};
}
