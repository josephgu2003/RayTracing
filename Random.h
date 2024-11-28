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
		double uni1 = randomDouble(-1, 1);
		double uni2 = randomDouble(-1, 1);

		if (uni1 == 0 && uni2 == 0)
			return Point(0, 0, 0);

		double theta, r;

		if (std::abs(uni1) > std::abs(uni2))
		{
			r = uni1;
			theta = pi / 4 * (uni2 / uni1);
		}
		else 
		{
			r = uni2;
			theta = pi / 2 - pi / 4 * (uni1 / uni2);
		}

		return r * Point(std::cos(theta), std::sin(theta), 0.0);
	}

	Point sampleUnitSphere()
	{
		double uni1 = randomDouble();
		double uni2 = randomDouble();
		double z = 1 - 2 * uni2;
		assert((1 - z * z) >= 0);
		double sinPhi = sqrt(1 - z * z);
		double x = cos(2 * pi * uni1) * sinPhi;
		double y = sin(2 * pi * uni1) * sinPhi;
		return Point(x, y, z);
	}

	// Malley's method
	Point sampleCosineHemisphere()
	{
		Point p = sampleUnitDisk();
		double z2 = 1 - p.x * p.x - p.y * p.y;
		assert(z2 >= 0);
		p.z = sqrt(z2);
		return p;
	}

	Point sampleCone(double radius, double height)
	{
		double r1 = randomDouble();
		double r2 = randomDouble();
		
		double cosMaxSquared = 1 - radius * radius / (height * height);
		assert(cosMaxSquared >= 1e-8);
		double cosMax = sqrt(cosMaxSquared);

		double z = 1 + r2 * (cosMax - 1);
		assert(z <= 1);
		double phi = 2 * pi * r1;
		double x = cos(phi) * sqrt(1 - z * z);
		double y = sin(phi) * sqrt(1 - z * z);
		return Point(x, y, z);
	}

private:
	std::uniform_real_distribution<double> distribution;
	std::mt19937 generator;
};
