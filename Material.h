#pragma once
#include <random>
#include "Hittable.h"
#include "Color.h"

class Material
{
public:
	virtual ~Material() = default;
	/// <summary>
	/// rayIn is the ray from the camera, but actually the outgoing ray physically. 
	/// Using the design choice to always scatter with attenuation, instead of scattering 
	/// probabilistically to model the albedo.
	/// </summary>
	virtual bool scatter(const Ray& rayIn, const Hit& hit, Color& attenuation, Ray& rayOut) = 0;
};

class Lambertian : public Material
{
private:
	std::uniform_real_distribution<double> distribution;
	std::shared_ptr<std::mt19937> generator;
	Color albedo;
public:
	Lambertian(const Color& albedo_, const std::shared_ptr<std::mt19937>& gen)
		: albedo(albedo_), distribution(-0.5, 0.5), generator(gen) {}

	bool scatter(const Ray& rayIn, const Hit& hit, Color& attenuation, Ray& rayOut) override
	{
		Vec out = sampleLambertian(hit.normal);
		rayOut = Ray(hit.pos, out);
		attenuation = albedo;
		return true;
	}

	/// <summary>
	/// Returns a normalized scatter direction.
	/// </summary>
	Vec sampleLambertian(const Vec& normal)
	{
		Vec vec;
		while (true)
		{
			vec = 2.0 * Vec(distribution(*generator), distribution(*generator), 
				distribution(*generator));

			if (glm::length(vec) < 1.0)
			{
				vec = glm::normalize(vec);
				break;
			}
		}
		vec = vec + normal;

		vec = nearZero(vec) ? normal : glm::normalize(vec);

		return (glm::dot(normal, vec) > 0) ? vec : -vec;
	}
};

class Metal : public Material
{
public:
	Metal(const Color& albedo, double fuzz, const std::shared_ptr<std::mt19937> gen) : albedo(albedo), fuzz(fuzz),
		distribution(-0.5, 0.5), generator(gen)
	{
		if (fuzz < 0.0 || fuzz > 1.0)
		{
			throw std::out_of_range("Fuzz out of range");
		}
	}

	bool scatter(const Ray& rayIn, const Hit& hit, Color& attenuation, Ray& rayOut) override
	{
		attenuation = albedo;
		// normal is unit vector, so is incident ray
		auto rayDir = rayIn.dir() - 2 * glm::dot(hit.normal, rayIn.dir()) * hit.normal;

		Vec vec;
		while (true)
		{
			vec = 2.0 * Vec(distribution(*generator), distribution(*generator), 
				distribution(*generator));

			if (glm::length(vec) < 1.0)
			{
				vec = glm::normalize(vec);
				break;
			}
		}

		rayDir = glm::normalize(rayDir) + fuzz * vec;
		rayOut = Ray(hit.pos, rayDir);

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

	bool scatter(const Ray& rayIn, const Hit& hit, Color& attenuation, Ray& rayOut) override
	{
		attenuation = Color(1, 1, 1);

		// frontface: ray is hitting material from outside
		double ri = hit.frontface ? 1.0 / refractionIndex : refractionIndex;

		double cosTheta = -glm::dot(rayIn.dir(), hit.normal);
		assert(cosTheta <= 1.0 && cosTheta >= 0.0);
		double sinTheta = sqrt(1 - cosTheta * cosTheta);

		// apply Schlick approximation stochastically (no way to do linear combination)
		if (ri * sinTheta > 1.0)
		{
			// reflection
			rayOut = Ray(hit.pos, reflect(rayIn.dir(), hit.normal));
		}
		else {
			// refraction
			rayOut = Ray(hit.pos, refract(rayIn.dir(), hit.normal, ri));
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
