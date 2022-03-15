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
		const float MAX_HEIGHT = 10.f;
		const float sinCos = sin(worldPosition.x * 0.1f) + cos(worldPosition.z * 0.1f);
		const float terrain = worldPosition.y - (MAX_HEIGHT * sinCos);

		const float cube = Cuboid(worldPosition, maths::Vec3f(-4.0f, 8.f, -4.0f), maths::Vec3f(12.f));
		const float sphere = Sphere(worldPosition, maths::Vec3f(-25.0f, 8.f, -4.0f), 16.0f);
		const float sphere2 = Sphere(worldPosition, maths::Vec3f(-25.0f, 8.f, -20.0f), 8.0f);
		const float sphere3 = Sphere(worldPosition, maths::Vec3f(-25.0f, 8.f, -32.5f), 4.0f);
		const float sphere4 = Sphere(worldPosition, maths::Vec3f(-25.0f, 8.f, -40.0f), 2.0f);
		const float sphere5 = Cuboid(worldPosition, maths::Vec3f(-25.0f, 8.f, -45.0f), maths::Vec3f(1.5f));

		return fmaxf(-cube, fminf(sphere, fminf(sphere2, fminf(sphere3, fminf(sphere4, fminf(sphere5, terrain))))));

		//return fmaxf(-cube, fminf(sphere, terrain));
	}
}
