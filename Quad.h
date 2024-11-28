#pragma once

#include "Hittable.h"

class Quad : public Hittable
{
public:
	Quad(const Point& Q, const Vec& u, const Vec& v, std::shared_ptr<Material> mat)
		: Q(Q), u(u), v(v), mat(mat)
	{
		n = glm::cross(u, v);
		assert(glm::length(n) > 1e-6);

		D = glm::dot(n, Q);
		w = n / glm::dot(n, n);
		area = glm::length(n);
	}

	bool hit(const Ray& ray, const Interval& interval, Hit& hit) const override
	{
		// find point of intersection on plane containing quad

		// n * ((ray.origin() + t * ray.dir()) - P) = 0

		double denom = glm::dot(n, ray.dir());

		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - glm::dot(n, ray.origin())) / denom;

		if (!interval.surrounds(t))
			return false;

		Vec P = ray.at(t);
		Vec p = P - Q;
		double alpha = glm::dot(-w, glm::cross(v, p));
		double beta = glm::dot(w, glm::cross(u, p));

		Interval unitInterval(0, 1);

		if (!unitInterval.surrounds(alpha) || !unitInterval.surrounds(beta))
			return false;

		hit.pos = P;
		hit.t = t;
		hit.mat = mat;
		hit.setFaceNormal(ray, glm::normalize(n));
		return true;
	}
	virtual double pdf(const Point& origin, const Point& dir) const
	{
		Ray ray(origin, dir);
		Hit hitpt;

		if (!hit(ray, Interval(0.001, std::numeric_limits<double>::max()), hitpt))
			return 0.0;

		double d2 = hitpt.t * hitpt.t;

		// I thought angles > 90 would return 0 pdf, but I guess
		// such angles would never happen according to the Hit conventions
		// Geometry that is occluded would simply not be hit
		double cosine = glm::dot(ray.dir(), -hitpt.normal);
		assert(cosine > 0);
		return d2 / (cosine * area);
	}

	virtual Vec randomSample(Random& rand, const Point& origin) const
	{
		Point P = Q + rand.randomDouble() * u + rand.randomDouble() * v;
		Vec vec = P - origin;

		assert(glm::length(vec) > 0);
		return glm::normalize(vec);
	}
private:
	Point Q;
	Vec u;
	Vec v;
	Vec n;
	Vec w;
	double D;
	std::shared_ptr<Material> mat;
	double area;
};