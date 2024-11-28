#pragma once
#include "Vec.h"
#include "Random.h"
#include "Onb.h"
#include "Hittable.h"

class Pdf
{
public:
	virtual ~Pdf() {}
	virtual double value(const Vec& vec) const = 0;
	virtual Vec generate(Random& rand) const = 0;
};

class SpherePdf : public Pdf
{
public:
	double value(const Vec& vec) const override
	{
		return 1.0 / (4.0 * pi);
	}

	Vec generate(Random& rand) const override
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

	double value(const Vec& vec) const override
	{
		assert(vecIsLength(vec, 1));

		double cosTheta = glm::dot(vec, onb.N());
		return cosTheta / pi;
	}

	Vec generate(Random& rand) const override
	{
		Vec vec = rand.sampleCosineHemisphere();

		return onb.localToWorld(vec);
	}

};

class HittablePdf : public Pdf
{
private:
	const Hittable& hittable;
	const Point& origin;
public:
	HittablePdf(const Hittable& hittable, const Point& origin) : hittable(hittable), origin(origin)
	{

	}
	double value(const Vec& vec) const override
	{
		return hittable.pdf(origin, vec);
	}
	Vec generate(Random& rand) const override
	{
		return hittable.randomSample(rand, origin);
	}
};

class MixturePdf : public Pdf
{
private:
	std::shared_ptr<Pdf> pdfA;
	std::shared_ptr<Pdf> pdfB;
	double ratio;
public:
	MixturePdf(const std::shared_ptr<Pdf>& pdfA, const std::shared_ptr<Pdf>& pdfB, double ratio = 0.5) : pdfA(pdfA), pdfB(pdfB), ratio(ratio)
	{

	}
	double value(const Vec& vec) const override
	{
		return ratio * pdfA->value(vec) + (1 - ratio) * pdfB->value(vec);
	}
	Vec generate(Random& rand) const override
	{
		return rand.randomDouble() > (1 - ratio) ? pdfA->generate(rand) : pdfB->generate(rand);
	}
};