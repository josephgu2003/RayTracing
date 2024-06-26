#pragma once
#include <random>
#include "Hittable.h"
#include "Color.h"
#include "Random.h"

// Handles the attenuation and scatter pdf values. The importance sampling is done elsewhere.
class Material
{
public:
	virtual ~Material() = default;
	/// <summary>
	/// rayIn is the ray from the camera, but actually the outgoing ray physically. 
	/// Using the design choice to always scatter with attenuation, instead of scattering 
	/// probabilistically to model the albedo.
	/// This function only tells if the ray scatters, and what is the attentuation in the rendering equation.
	/// </summary>
	virtual bool scatter(const Ray& rayIn, const Hit& hit, Random& rand, Color& attenuation) = 0;

	virtual Color emitted(const Ray& rayIn, const Hit& hit, Random& rand)
	{
		return Color(0, 0, 0);
	}
	/// <summary>
	/// Gonna assume the hit normal is facing against the ray, due to the convention in Hittables.
	/// This is the scattering pdf in the rendering equation.
	/// </summary>
	virtual double scatteringPDF(const Ray& rayIn, const Hit& hit, const Ray& scattered)
	{
		return 0.0f;
	}
};

class Lambertian : public Material
{
private:
	Color albedo;
public:
	Lambertian(const Color& albedo_)
		: albedo(albedo_) {}

	bool scatter(const Ray& rayIn, const Hit& hit, Random& rand, Color& attenuation) override
	{
		//Vec out = sampleLambertian(hit.normal, rand);
	//	rayOut = Ray(hit.pos, out);
		attenuation = albedo;
		return true;
	}

	/// <summary>
	/// Returns a normalized scatter direction.
	/// </summary>
	Vec sampleLambertian(const Vec& normal, Random& rand)
	{
		Vec vec = rand.sampleUnitSphere();
		
		vec = vec + normal;

		vec = nearZero(vec) ? normal : glm::normalize(vec);

		double D = glm::dot(normal, vec);
		assert(D >= 0);

		return (D > 0) ? vec : -vec;
	}

	double scatteringPDF(const Ray& rayIn, const Hit& hit, const Ray& scattered) override
	{
		return max(glm::dot(scattered.dir(), hit.normal) / pi, 0.0);
	}
};

class Metal : public Material
{
public:
	Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz)
	{
		if (fuzz < 0.0 || fuzz > 1.0)
		{
			throw std::out_of_range("Fuzz out of range");
		}
	}

	bool scatter(const Ray& rayIn, const Hit& hit, Random& rand, Color& attenuation) override
	{
		attenuation = albedo;
		// normal is unit vector, so is incident ray
		auto rayDir = rayIn.dir() - 2 * glm::dot(hit.normal, rayIn.dir()) * hit.normal;

	//	Vec vec = rand.sampleUnitSphere();

	//	rayDir = glm::normalize(rayDir) + fuzz * vec;
	//	rayOut = Ray(hit.pos, rayDir);

		return true;
	}
private:
	Color albedo;
	double fuzz;
	std::uniform_real_distribution<double> distribution;
	std::shared_ptr<std::mt19937> generator;
};


class Dielectric : public Material
{
public:
	Dielectric(double refractionIndex) : refractionIndex(refractionIndex)
	{
	}

	bool scatter(const Ray& rayIn, const Hit& hit, Random& rand, Color& attenuation) override
	{
		attenuation = Color(1, 1, 1);

		// frontface: ray is hitting material from outside
		double ri = hit.frontface ? 1.0 / refractionIndex : refractionIndex;

		double cosTheta = -glm::dot(rayIn.dir(), hit.normal);
		assert(cosTheta <= 1.0 && cosTheta >= 0.0);
		double sinTheta = sqrt(1 - cosTheta * cosTheta);

		// apply Schlick approximation stochastically (no way to do linear combination)
		if (ri * sinTheta > 1.0 || reflectance(cosTheta, ri) > rand.randomDouble())
		{
			// reflection
	//		rayOut = Ray(hit.pos, reflect(rayIn.dir(), hit.normal));
		}
		else {
			// refraction
		//	rayOut = Ray(hit.pos, refract(rayIn.dir(), hit.normal, ri));
		}

		return true;
	}
private:
	double refractionIndex; // ratio of material index over enclosing media index

	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

class Emissive : public Material
{
private:
	Color color;
public:
	Emissive(const Color& color) : color(color)
	{

	}
	bool scatter(const Ray& rayIn, const Hit& hit, Random& rand, Color& attenuation) override
	{
		return false;
	}
	Color emitted(const Ray& rayIn, const Hit& hit, Random& rand) override
	{
		if (!hit.frontface)
			return Color(0, 0, 0);
		return color;
	}
};
