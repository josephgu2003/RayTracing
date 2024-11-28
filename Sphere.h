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
		double pdf(const Point& origin, const Point& dir) const override
		{
			Hit hitpt;

			if (!hit(Ray(origin, dir), Interval(0.001, std::numeric_limits<double>::max()), hitpt))
				return 0;

			double height = glm::length(center - origin);
			double cosMaxSquared = 1 - radius * radius / (height * height);
			assert(cosMaxSquared >= -1e-8);
			double cosMax = sqrt(std::max(0.0, cosMaxSquared));
			return 1 / (2 * pi * (1 - cosMax));
		}

		Vec randomSample(Random& rand, const Point& origin) const override
		{
			Vec toSphere = center - origin;
			double d = glm::length(toSphere);
			Vec dir = rand.sampleCone(radius, d);
			Onb onb(glm::normalize(toSphere));
			return glm::normalize(onb.localToWorld(dir));
		}

	private:
		Point center;
		double radius;
		std::shared_ptr<Material> mat;
};
