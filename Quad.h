#pragma once

#include "Hittable.h"

class Quad : public Hittable
{
public:
	Quad(const Point& Q, const Vec& u, const Vec& v, std::shared_ptr<Material> mat)
		: Q(Q), u(u), v(v), mat(mat)
	{
	}

	bool hit(const Ray& ray, const Interval& interval, Hit& hit) const override
	{
		// find point of intersection on plane containing quad

		// n * ((ray.origin() + t * ray.dir()) - P) = 0

		Vec n = glm::cross(u, v);
		double D = glm::dot(n, Q);
		double denom = glm::dot(n, ray.dir());

		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - glm::dot(n, ray.origin())) / denom;

		if (!interval.surrounds(t))
			return false;

		Vec P = ray.at(t);
		Vec p = P - Q;
		Vec w = n / glm::dot(n, n);
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
private:
	Point Q;
	Vec u;
	Vec v;
	std::shared_ptr<Material> mat;
};