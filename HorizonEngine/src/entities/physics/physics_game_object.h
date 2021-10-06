#pragma once

#include "../renderable_game_object.h"
#include "rigid_body.h"

namespace hrzn::entity::physics
{
	class PhysicsGameObject : public RenderableGameObject
	{
	public:
		PhysicsGameObject();
		~PhysicsGameObject();

		bool initialize(std::string label, const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		void setMass(float mass);
		void setDrag(float drag);

		float getMass();
		float getDrag();

		RigidBody* getRigidBody();

		void update(float deltaTime) override;
		void updateWorldSpaceBoundingBox();
		BoundingOrientedBox getWorldSpaceBoundingBox();

	private:
		RigidBody           m_rigidBody;

		BoundingOrientedBox m_worldSpaceBoundingBox;
	};
}
