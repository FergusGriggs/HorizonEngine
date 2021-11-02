
//Function implementations for the Camera class

#include "camera_game_object.h"

#include "../input/input_manager.h"

namespace hrzn::entity
{
	CameraGameObject::CameraGameObject() :
		GameObject(),
		
		m_view(),
		m_projection(),
		
		m_fov(90.0f),
		m_aspectRatio(1.0f),
		m_nearZ(0.1f),
		m_farZ(10000.0f),
		
		m_mouseToWorldVectorDirection(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),

		m_relativeObject(nullptr),
		m_relativePosition(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
	{
		m_label = "Camera";
		m_type = GameObject::Type::eCamera;
		m_relativeObject = nullptr;
	}

	void CameraGameObject::update(float deltaTime)
	{
		if (m_relativeObject != nullptr)
		{
			m_transform.setPosition(m_relativeObject->getTransform().getPositionVector() + m_relativeObject->getTransform().getRightVector() * XMVectorGetX(m_relativePosition) + m_relativeObject->getTransform().getUpVector() * XMVectorGetY(m_relativePosition) + m_relativeObject->getTransform().getFrontVector() * XMVectorGetZ(m_relativePosition));
		}
		if (m_followingTrack)
		{
			m_objectTrack->follow(this, deltaTime);
		}
	}


	void CameraGameObject::setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		m_fov = fovDegrees;
		m_aspectRatio = aspectRatio;
		m_nearZ = nearZ;
		m_farZ = farZ;
		float fovRadians = (fovDegrees / 180.0f) * XM_PI;
		m_projection = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	void CameraGameObject::zoom(float fovDiff)
	{
		m_fov += fovDiff;
		if (m_fov < 10.0f)
		{
			m_fov = 10.0f;
		}
		else if (m_fov > 150.0f)
		{
			m_fov = 150.0f;
		}
		float fovRadians = (m_fov / 180.0f) * XM_PI;
		m_projection = XMMatrixPerspectiveFovLH(fovRadians, m_aspectRatio, m_nearZ, m_farZ);
	}

	void CameraGameObject::setFOV(float fov)
	{
		m_fov = fov;
		float fovRadians = (fov / 180.0f) * XM_PI;
		m_projection = XMMatrixPerspectiveFovLH(fovRadians, m_aspectRatio, m_nearZ, m_farZ);
	}

	float CameraGameObject::getFOV() const
	{
		return m_fov;
	}

	const XMMATRIX& CameraGameObject::getViewMatrix() const
	{
		return m_view;
	}

	const XMMATRIX& CameraGameObject::getProjectionMatrix() const
	{
		return m_projection;
	}

	void CameraGameObject::updateView()
	{
		m_view = XMMatrixLookToLH(m_transform.getPositionVector(), m_transform.getFrontVector(), m_transform.getUpVector());
	}

	void CameraGameObject::updateMouseToWorldVectorDirection()
	{
		XMMATRIX inverseView = XMMatrixInverse(nullptr, m_view);
		XMMATRIX inverseProjection = XMMatrixInverse(nullptr, m_projection);

		XMVECTOR clipSpaceVector = XMVectorSet(InputManager::it().getMousePosNDC().x, InputManager::it().getMousePosNDC().y, 0.0f, 0.0f);

		XMVECTOR viewSpaceVector = XMVector3Transform(clipSpaceVector, inverseProjection);

		XMVECTOR worldSpaceVector = XMVector3Transform(viewSpaceVector, inverseView);

		m_mouseToWorldVectorDirection = XMVector3Normalize(worldSpaceVector - m_transform.getPositionVector());
	}

	const XMVECTOR& CameraGameObject::getMouseToWorldVectorDirection() const
	{
		return m_mouseToWorldVectorDirection;
	}

	XMFLOAT2 CameraGameObject::getNDCFrom3DPos(XMVECTOR objectPosition) const
	{
		XMVECTOR viewSpaceVector = XMVector3Transform(objectPosition, m_view);

		XMVECTOR clipSpaceVector = XMVector3Transform(viewSpaceVector, m_projection);

		float w = XMVectorGetW(clipSpaceVector);
		return XMFLOAT2(XMVectorGetX(clipSpaceVector) / w, XMVectorGetY(clipSpaceVector) / w);
	}

	void CameraGameObject::setRelativeObject(const GameObject* relativeObject, const XMVECTOR& relativePosition)
	{
		m_relativeObject = relativeObject;
		m_relativePosition = relativePosition;
	}

	void CameraGameObject::setRelativeObject(const GameObject* relativeObject, const XMFLOAT3& relativePosition)
	{
		setRelativeObject(relativeObject, XMLoadFloat3(&relativePosition));
	}

	void CameraGameObject::unsetRelativeObject()
	{
		m_relativeObject = nullptr;
	}

	bool CameraGameObject::isUsingRelativeCamera()
	{
		return m_relativeObject != nullptr;
	}
}
