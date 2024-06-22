#pragma once
#include <glm/glm.hpp>

using Vec = glm::vec<3, double>;

static constexpr Vec ZERO_VEC(0, 0, 0);
static constexpr double VEC_EPS = 1e-8;

inline bool nearZero(const Vec& vec)
{
	return (std::fabs(vec.x - ZERO_VEC.x) < VEC_EPS) && (std::fabs(vec.y - ZERO_VEC.y) < VEC_EPS)
		&& (std::fabs(vec.z - ZERO_VEC.z) < VEC_EPS);
}

inline Vec reflect(const Vec& ray0, const Vec& norm)
{
	assert(fabs(1.0 - dot(norm, norm)) < 1e-8);
	return ray0 - 2 * glm::dot(ray0, norm) * norm;
}

/// <summary>
/// Expects both input vectors to be unit vectors. Outputs a unit vector by mathematical principles.
/// </summary>
inline Vec refract(const Vec& ray0, const Vec& norm, double eta1OverEta0)
{
	assert(fabs(1.0 - glm::dot(ray0, ray0)) < 1e-8);
	assert(fabs(1.0 - glm::dot(norm, norm)) < 1e-8);
	Vec ray1Comp = eta1OverEta0 * (ray0 - (glm::dot(ray0, norm) * norm));

	// why does the website use fabs? I think the math assumes the angles are acute,
	// and fabs prevents exceptions when the eta1OverEta0 is out of range??
	Vec ray1Proj = -sqrt(fabs(1.0 - glm::dot(ray1Comp, ray1Comp))) * norm;

	Vec result = ray1Comp + ray1Proj;
	assert(fabs(1.0 - glm::dot(result, result)) < 1e-6);
	return result;
}
