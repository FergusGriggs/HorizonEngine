#pragma once

#include "renderable_game_object.h"

#include "../physics/rigid_body.h"

namespace hrzn::entity
{
	class PhysicsGameObject : public RenderableGameObject
	{
	public:
		PhysicsGameObject();
		~PhysicsGameObject();

		bool initialise(std::string label, const std::string& filePath);

		void setMass(float mass);
		void setDrag(float drag);

		float getMass();
		float getDrag();

		physics::RigidBody* getRigidBody();

		void update(float deltaTime) override;
		void updateWorldSpaceBoundingBox();
		BoundingOrientedBox getWorldSpaceBoundingBox();

	private:
		physics::RigidBody  m_rigidBody;

		BoundingOrientedBox m_worldSpaceBoundingBox;
	};
}
