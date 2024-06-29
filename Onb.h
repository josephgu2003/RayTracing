#pragma once
#include "Vec.h"

// Orthonormal basis
class Onb
{
private:
	Vec n, u, v;
public:
	Onb(const Vec& norm) {
		assert(vecIsLength(norm, 1));

		n = norm;

		Vec a = fabs(n.x) > 0.9 ? Vec(0, 1, 0) : Vec(1, 0, 0);

		u = glm::normalize(glm::cross(n, a));
		assert(vecIsLength(u, 1));

		v = glm::normalize(glm::cross(n, u));
		assert(vecIsLength(v, 1));
	}

	Vec localToWorld(const Vec& local) const
	{
		return local.x * u + local.y * v + local.z * n;
	}

	Vec N() const
	{
		return n;
	}
};