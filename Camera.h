#pragma once
#include <iostream>
#include <stdexcept>

#include "Color.h"
#include "HittableList.h"
#include "Random.h"
#include "RayTracing.h"

struct CamParams
{
public:
	double aspectRatio = 16.0 / 9.0;
	double focalDist = 5.0;
	double defocusAngle = 0.2;
	int samplesPerPixel = 100;
	int maxDepth = 8;
	int imgWidth = 400;
	double vFov = 50.0;
	Point pos = Point(0.0, 0.0, 0.0);
	Point lookAt = Point(0.0, 0.0, -1.0);
	Vec upDir = Vec(0.0, 1.0, 0.0);
	Color background = Color(0, 0, 0);
};

class Camera
{
public:
	Camera(CamParams params, Random random)
	{
		this->samplesPerPixel = params.samplesPerPixel;
		this->maxDepth = params.maxDepth;
		this->rand = random;
		this->focusDist = params.focalDist;
		this->defocusAngle = params.defocusAngle;
		this->background = params.background;

		// IMG PARAMS
		// set up image pixel dims based on aspect ratio
		this->imgWidth = params.imgWidth;

		this->imgHeight = int(imgWidth / params.aspectRatio);

		if (imgHeight < 1)
		{
			throw std::invalid_argument("Image height < 1");
		}

		// CAMERA PARAMS
		// 
		// (0, 1, 0) -> Up
		// 
		// pixel locations are at center of pixel squares
		// 
		// use true ratio of width / height pixels for precision
		auto viewportHeight = 2 * focusDist * tan(degToRad(params.vFov / 2.0));
		auto viewportWidth = viewportHeight * (imgWidth / (double)imgHeight);
		this->cameraOrigin = params.pos;

		// spanning vectors of viewport
		auto forward = glm::normalize(params.lookAt - params.pos);

		auto viewportU = glm::cross(forward, params.upDir);
		viewportU = viewportWidth * glm::normalize(viewportU);

		auto viewportV = glm::cross(forward, viewportU);
		viewportV = viewportHeight * glm::normalize(viewportV);

		// deltas between pixels
		this->deltaU = viewportU / (double)imgWidth;
		this->deltaV = viewportV / (double)imgHeight;

		assert(fabs(1.0 - glm::dot(deltaU - deltaV, deltaU - deltaV)) < 1e-6);

		double defocusRadius = tan(degToRad(defocusAngle)) * focusDist;
		this->defocusU = glm::normalize(deltaU) * defocusRadius;
		this->defocusV = glm::normalize(deltaV) * defocusRadius;

		// position of 00 pixel
		auto viewportTopLeft = cameraOrigin + focusDist * forward
			- viewportU / 2.0 - viewportV / 2.0;
		this->pixel00Pos = viewportTopLeft + 0.5 * deltaU + 0.5 * deltaV;

		img.resize(imgHeight * imgWidth *  3);
	}

	template <typename T>
	Color rayColor(const Ray& ray, T hittables, int depth)
	{
		if (depth <= 0)
			return Color(0, 0, 0);

		Interval interval(0.0001, 100);
		Hit hit;
		if (hittables.hit(ray, interval, hit))
		{
			Color emitted = hit.mat->emitted(ray, hit, rand);

			Color att;
			Ray out;

			if (!hit.mat->scatter(ray, hit, rand, att, out))
				return emitted;

			return emitted + (rayColor(out, hittables, depth - 1)) * att;
		}
		else {
			return background;
		}
	}
	double linearToGamma(double linear)
	{
		return linear > 0.0 ? sqrt(linear) : 0.0;
	}

	const std::vector<unsigned char> render(const HittableList& hittables)
	{
		for (int row = 0; row < imgHeight; row++)
		{
			std::cout << "Scanlines remaining: " << (imgHeight - row) << ' ' << std::endl;

			for (int col = 0; col < imgWidth; col++)
			{
				Color color(0, 0, 0);
				for (int s = 0; s < samplesPerPixel; s++)
				{
					auto ray = sampleRayToPixel(row, col);
					color += rayColor(ray, hittables, maxDepth);
				}
				color /= samplesPerPixel;
				color = glm::clamp(color, 0.0, 0.999);
				img[3 * (row * imgWidth + col)] = linearToGamma(color.x) * 256;
				img[3 * (row * imgWidth + col) + 1] = linearToGamma(color.y) * 256;
				img[3 * (row * imgWidth + col) + 2] = linearToGamma(color.z) * 256;
			}
		}

		return img;
	}

	int imageWidth() const { return imgWidth; }
	int imageHeight() const { return imgHeight; }

	Ray sampleRayToPixel(int row, int col)
	{
		auto pixelPos = pixel00Pos + (row + rand.randomDouble(-0.5, 0.5)) * deltaV +
			(col + rand.randomDouble(-0.5, 0.5)) * deltaU;

		auto rayOrigin = (defocusAngle <= 0.0) ? cameraOrigin : defocusDiskSample();
		auto rayDir = pixelPos - rayOrigin;

		return Ray(rayOrigin, rayDir);
	}

	Point defocusDiskSample()
	{
		auto randOnDisk = rand.sampleUnitDisk();
		return cameraOrigin + randOnDisk.x * defocusU + randOnDisk.y * defocusV;
	}

private:
	std::vector<unsigned char> img;
	int imgWidth, imgHeight;
	Point pixel00Pos, cameraOrigin;
	Vec deltaV, deltaU;
	int samplesPerPixel;
	int maxDepth;

	double focusDist; // we assume focal length = focusDist
	double defocusAngle; // angle of cone formed by lens and center point of focal plane
	Vec defocusV, defocusU;

	Color background;

	Random rand;
};
