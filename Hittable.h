#pragma once
#include "Ray.h"
#include "Interval.h"
#include <memory>

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
};
