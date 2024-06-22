#pragma once
#include <glm/glm.hpp>
#include "Point.h"
#include "Vec.h"

using namespace glm;

class Ray
{
	public:
		Ray() : orig(Point(0, 0, 0)), direction(Vec(1, 0, 0)) 
		{

		}

		Ray(const Point& origin, const Vec& dir) : orig(origin), direction(glm::normalize(dir))
		{

		}
		const Point& origin() const { return orig; }
		const Vec& dir() const { return direction; }
		Point at(double t) const
		{
			return orig + direction * t;
		}
	private:
		Point orig;
		Vec direction;
};