#include "density.h"

#include "../../../maths/vec2.h"

namespace hrzn::scene::terrain
{
	// ----------------------------------------------------------------------------

	float Sphere(const maths::Vec3f& worldPosition, const maths::Vec3f& origin, float radius)
	{
		return maths::Vec3f::length(worldPosition - origin) - radius;
	}

	// ----------------------------------------------------------------------------

	float Cuboid(const maths::Vec3f& worldPosition, const maths::Vec3f& origin, const maths::Vec3f& halfDimensions)
	{
		const maths::Vec3f& local_pos = worldPosition - origin;
		const maths::Vec3f& pos = local_pos;

		const maths::Vec3f& d = maths::Vec3f::abs(pos) - halfDimensions;
		const float m = fmaxf(d.x, fmaxf(d.y, d.z));
		return fminf(m, maths::Vec3f::length(maths::Vec3f::max(d, maths::Vec3f(0.0f))));
	}

	// ----------------------------------------------------------------------------

	float Density_Func(const maths::Vec3f& worldPosition)
	{
		const float MAX_HEIGHT = 20.f;
		const float sinCos = sin(worldPosition.x * 0.1f) + cos(worldPosition.z * 0.1f);
		const float terrain = worldPosition.y - (MAX_HEIGHT * sinCos);

		const float cube = Cuboid(worldPosition, maths::Vec3f(-4., 10.f, -4.f), maths::Vec3f(12.f));
		const float sphere = Sphere(worldPosition, maths::Vec3f(15.f, 2.5f, 1.f), 16.f);

		return fmaxf(-cube, fminf(sphere, terrain));
	}
}
