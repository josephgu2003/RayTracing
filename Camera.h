#pragma once
#include <iostream>
#include <stdexcept>
#include <chrono>

#include "Color.h"
#include "HittableList.h"
#include "Random.h"
#include "RayTracing.h"
#include "Pdf.h"
#include "Material.h"

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
	double mixturePDFRatio = 0.5;
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

		assert(fabs(glm::dot(deltaU, deltaU) - glm::dot(deltaV, deltaV)) < 1e-6);

		double defocusRadius = tan(degToRad(defocusAngle)) * focusDist;
		this->defocusU = glm::normalize(deltaU) * defocusRadius;
		this->defocusV = glm::normalize(deltaV) * defocusRadius;

		// position of 00 pixel
		auto viewportTopLeft = cameraOrigin + focusDist * forward
			- viewportU / 2.0 - viewportV / 2.0;
		this->pixel00Pos = viewportTopLeft + 0.5 * deltaU + 0.5 * deltaV;

		img.resize(imgHeight * imgWidth *  3);

		this->mixturePDFRatio = params.mixturePDFRatio;
	}

	Color rayColor(const Ray& ray, const HittableList& hittables, const Hittable& lights, int depth)
	{
		if (depth <= 0)
			return Color(0, 0, 0);

		Interval interval(0.0001, 100);
		Hit hit;

		if (hittables.hit(ray, interval, hit))
		{
			Color emitted = hit.mat->emitted(ray, hit, rand);

			const ScatterRecord scatterRecord = hit.mat->scatter(ray, hit, rand);

			if (!scatterRecord.scattered)
				return emitted;

			if (scatterRecord.skipPdf)
			{
				// rendering equation is kind of in here
				return emitted + (rayColor(scatterRecord.skipPdfRay, hittables, lights, depth - 1))
					* scatterRecord.attenuation;
			} 

			// generate scattered ray based on importance sampling
			const MixturePdf surfacePdf(std::make_shared<HittablePdf>(lights, hit.pos), scatterRecord.pdfPtr, mixturePDFRatio);
			Ray out(hit.pos, surfacePdf.generate(rand));
			double samplingPDF = surfacePdf.value(out.dir());
			double scatteringPDF = scatterRecord.pdfPtr->value(out.dir());
			assert(samplingPDF != 0);
			assert(scatteringPDF != 0);

			// rendering equation is kind of in here
			return emitted + (rayColor(out, hittables, lights, depth - 1)) * scatterRecord.attenuation * scatteringPDF / samplingPDF;
		}
		else {
			return background;
		}

	}
	Color aces_approx(const Color& v)
		{
			Color val = v * 0.6;
			double a = 2.51f;
			double b = 0.03f;
			double c = 2.43f;
			double d = 0.59f;
			double e = 0.14f;
			return glm::clamp((val * (a * val + b)) / (val * (c * val + d) + e), 0.0, 1.0);
		}

	double linearToGamma(double linear)
	{
		return linear > 0.0 ? pow(linear, 1.0 / 2.2) : 0.0;
	}

	const std::vector<unsigned char> render(const HittableList& hittables, const Hittable& lights)
	{
		auto start = std::chrono::high_resolution_clock::now();
		for (int row = 0; row < imgHeight; row++)
		{
			std::cout << "Scanlines remaining: " << (imgHeight - row) << ' ' << std::endl;

			for (int col = 0; col < imgWidth; col++)
			{
				Color color(0, 0, 0);
				for (int s = 0; s < samplesPerPixel; s++)
				{
					auto ray = sampleRayToPixel(row, col);
					color += rayColor(ray, hittables, lights, maxDepth);
				}
				color /= samplesPerPixel;
				color = aces_approx(color);
				img[3 * (row * imgWidth + col)] = linearToGamma(color.x) * 255;
				img[3 * (row * imgWidth + col) + 1] = linearToGamma(color.y) * 255;
				img[3 * (row * imgWidth + col) + 2] = linearToGamma(color.z) * 255;
			}
		}

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = duration_cast<std::chrono::seconds>(stop - start);
		std::cout << duration.count() << std::endl;

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

	double mixturePDFRatio;
};
