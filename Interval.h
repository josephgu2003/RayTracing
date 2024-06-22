#pragma once
#include <limits>

class Interval
{
public:
	double min, max;
	Interval() : min(+std::numeric_limits<double>::max()), max(-std::numeric_limits<double>::min())
	{

	}

	Interval(double min, double max) : min(min), max(max)
	{

	}

	bool contains(double t) const
	{
		return min <= t && t <= max;
	}
	bool surrounds(double t) const
	{
		return min < t && t < max;
	}
};
