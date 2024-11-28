#pragma once
#include "Ray.h"
#include "Interval.h"
#include <memory>
#include "Random.h"

class Material;

class Hit
{
public:
	Point pos;
	Vec normal;
	double t;
	bool frontface;
	std::shared_ptr<Material> mat;

	void setFaceNormal(const Ray& ray, const Vec& outNorm)
	{
		frontface = glm::dot(ray.dir(), outNorm) < 0;
		normal = frontface ? outNorm : -outNorm;
	}
};

class Hittable
{
	public:
		virtual ~Hittable() = default;

		virtual bool hit(const Ray& ray, const Interval& interval, Hit& hit) const = 0;

		// Watch out for divide by zero error if pdf = 0.
		// Just make sure that randomSample always returns a direction
		// that actually hits the surface of this
		virtual double pdf(const Point& origin, const Point& dir) const = 0;
		/// <summary>
		/// Sample random direction to hittable from origin.
		/// </summary>
		/// <param name="rand"></param>
		/// <returns></returns>
		virtual Vec randomSample(Random& rand, const Point& origin) const = 0;
};
