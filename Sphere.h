#pragma once
#include "Hittable.h"
#include <iostream>
#include "Material.h"

class Sphere : public Hittable
{
	public:
		Sphere(const Point& center_, double radius_, const std::shared_ptr<Material>& mat)
			: center(center_), radius(radius_), mat(mat)
		{

		}

		bool hit(const Ray& ray, const Interval& interval, Hit& hit) const override
		{
			auto& d = ray.dir();
			auto& q = ray.origin();

			auto a = glm::dot(d, d);

			auto qc = q - center;
			auto b = 2.0f * glm::dot(d, qc);
			auto c = glm::dot(qc, qc) - radius * radius;

			auto discriminant = (b * b - 4.0f * a * c);
			
			if (discriminant < 0)
				return false;

			auto t = ( - b - sqrt(discriminant)) / 2 / a;

			if (!interval.surrounds(t))
			{
				t = (-b + sqrt(discriminant)) / 2 / a;

				if (!interval.surrounds(t))
					return false;
			}

			hit.t = t;
			hit.pos = ray.at(hit.t);
			hit.mat = mat;
		    auto outNorm = glm::normalize(hit.pos - center);

			hit.setFaceNormal(ray, outNorm);

			return true;
		}

	private:
		Point center;
		double radius;
		std::shared_ptr<Material> mat;
};
