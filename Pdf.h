#pragma once
#include "Vec.h"
#include "Random.h"
#include "Onb.h"

class Pdf
{
public:
	virtual ~Pdf() {}
	virtual double value(const Vec& vec) = 0;
	virtual Vec generate(Random& rand) = 0;
};

class SpherePdf : public Pdf
{
public:
	double value(const Vec& vec) override
	{
		return 1.0 / (4.0 * pi);
	}

	Vec generate(Random& rand) override
	{
		return rand.sampleUnitSphere();
	}
};

class CosinePdf : public Pdf
{
private:
	Onb onb;
public:
	CosinePdf(const Vec& norm) : onb(norm)
	{
	}

	double value(const Vec& vec) override
	{
		assert(vecIsLength(vec, 1));

		double cosTheta = glm::dot(vec, onb.N());
		return cosTheta / pi;
	}

	Vec generate(Random& rand) override
	{
		Vec vec = rand.sampleCosineHemisphere();

		return onb.localToWorld(vec);
	}

};