#pragma once
#include <iostream>
#include <stdexcept>
#include <random>

#include "Color.h"
#include "HittableList.h"

class Camera
{
public:
	Camera(double aspectRatio, double focalLength, int samplesPerPixel, int maxDepth)
	{
		this->samplesPerPixel = samplesPerPixel;
		this->maxDepth = maxDepth;
		distribution = std::uniform_real_distribution<>(-0.5, 0.5);

		// IMG PARAMS
		// set up image pixel dims based on aspect ratio
		imgWidth = 400;

		imgHeight = int(imgWidth / aspectRatio);

		if (imgHeight < 1)
		{
			throw std::invalid_argument("Image height < 1");
		}

		// CAMERA PARAMS
		// 
		// (0, 1, 0) -> Up
		// (1, 0, 0) -> Right
		// (0, 0, -1) -> Forwards
		// 
		// pixel locations are at center of pixel squares
		// 
		// use true ratio of width / height pixels for precision
		auto viewportHeight = 0.1;
		auto viewportWidth = viewportHeight * (imgWidth / (double)imgHeight);
		cameraOrigin = Point(0, 0, 0);

		// spanning vectors of viewport
		auto viewportU = Vec(viewportWidth, 0, 0);
		auto viewportV = Vec(0, -viewportHeight, 0);

		// deltas between pixels
		deltaU = viewportU / (double)imgWidth;
		deltaV = viewportV / (double)imgHeight;

		// position of 00 pixel
		auto viewportTopLeft = cameraOrigin + focalLength * Vec(0, 0, -1)
			- viewportU / 2.0 - viewportV / 2.0;
		pixel00Pos = viewportTopLeft + 0.5 * deltaU + 0.5 * deltaV;

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
			Color att;
			Ray out;
			hit.mat->scatter(ray, hit, att, out);
			return (rayColor(out, hittables, depth - 1)) * att;
			//return glm::clamp(hit.normal, 0.0, 1.0);
		}

		auto unit_direction = ray.dir();
		auto a = 0.5 * (unit_direction.y + 1.0);
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
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
		auto pixelPos = pixel00Pos + (row + distribution(generator)) * deltaV +
			(col + distribution(generator)) * deltaU;
		auto rayDir = pixelPos - cameraOrigin;

		return Ray(cameraOrigin, rayDir);
	}

private:
	std::vector<unsigned char> img;
	int imgWidth, imgHeight;
	Point pixel00Pos, cameraOrigin;
	Vec deltaV, deltaU;
	int samplesPerPixel;
	int maxDepth;

	std::uniform_real_distribution<double> distribution;
	std::mt19937 generator;
};
