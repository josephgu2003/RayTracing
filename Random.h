#pragma once
#include <random>
#include "Point.h"
#include "RayTracing.h"

class Random
{
public:
	Random() : generator(), distribution(0.0, 1.0)
	{
	}

	Random(int seed) : generator(seed), distribution(0.0, 1.0)
	{
	}

	double randomDouble()
	{
		return distribution(generator);
	}

	double randomDouble(double lower, double upper)
	{
		return lower + randomDouble() * (upper - lower);
	}

	Point sampleUnitDisk()
	{
		Point pt;
		while (true)
		{
			// random vector on box of [-1, 1]
			pt = Point(randomDouble(-1.0, 1.0), randomDouble(-1.0, 1.0), 0.0);

			if (glm::length(pt) < 1.0)
			{
				pt = glm::normalize(pt);
				return pt;
			}
		}
	}

	Point sampleUnitSphere()
	{
		double uni1 = randomDouble();
		double uni2 = randomDouble();
		double z = 1 - 2 * uni2;
		assert((1 - z * z) <= 1.00001);
		assert((1 - z * z) >= 0.00000);
		double sinPhi = sqrt(1 - z * z);
		double x = cos(2 * pi * uni1) * sinPhi;
		double y = sin(2 * pi * uni1) * sinPhi;
		return Point(x, y, z);
	}

private:
	std::uniform_real_distribution<double> distribution;
	std::mt19937 generator;
};
