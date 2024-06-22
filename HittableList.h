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
private:
	std::vector<std::shared_ptr<Hittable>> hittables;
};
