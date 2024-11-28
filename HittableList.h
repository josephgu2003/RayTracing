#pragma once
#include "Hittable.h"
#include <vector>
#include <memory>

class HittableList : public Hittable
{
public:
	bool hit(const Ray& ray, const Interval& interval, Hit& hit) const override
	{
		auto closestHit = interval.max;
		auto hitValid = false;

		for (auto& hittable : hittables)
		{
			Hit thisHit;

			if (hittable->hit(ray, Interval(interval.min, closestHit), thisHit))
			{
				closestHit = thisHit.t;
				hit = thisHit;
				hitValid = true;
			}
		}
		return hitValid;
	}
	void add(std::shared_ptr<Hittable> hittable)
	{
		hittables.push_back(hittable);
	}
	double pdf(const Point& origin, const Point& dir) const
	{
		int len = hittables.size();
		double sum = 0;

		for (auto& hittable : hittables)
		{
			sum += hittable->pdf(origin, dir);

		}

		return sum / len;
	}
	
	Vec randomSample(Random& rand, const Point& origin) const
	{
		int len = hittables.size();
		int index = (int) rand.randomDouble(0, len);

		return hittables.at(index)->randomSample(rand, origin);
	}
private:
	std::vector<std::shared_ptr<Hittable>> hittables;
};
