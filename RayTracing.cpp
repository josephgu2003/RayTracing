// RayTracing.cpp : Defines the entry point for the application.
//

#include "RayTracing.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Point.h"
#include "Ray.h"
#include "Color.h"
#include "Sphere.h"
#include "HittableList.h"
#include "Camera.h"

using namespace std;

int raytrace()
{
	CamParams params;
	params.samplesPerPixel = 700;
	params.imgWidth = 300;
	params.vFov = 10.0;
	params.pos = Point(0, 2, 0);
	params.lookAt = Point(0, 0, -5);
	params.defocusAngle = -1;
	params.maxDepth = 8;
	Camera cam(params, Random(1));
	// SCENE
	HittableList hittables;

	auto redMat = std::make_shared<Lambertian>(Color(1, 0.1, 0.1));
	auto metalMat = std::make_shared<Metal>(Color(0.7, 1.0, 1) * 0.4, 0.0);
	auto whiteMat = std::make_shared<Lambertian>(Color(1, 1.0, 1));
	auto greenMat = std::make_shared<Lambertian>(Color(0.8, 0.8, 1) * 0.4);
	auto glassMat = std::make_shared<Dielectric>(1.5);

	// environment
	hittables.add(std::make_shared<Sphere>(Point(0, -1000.3, -5), 1000, greenMat));
	hittables.add(std::make_shared<Sphere>(Point(0, 0, 0), 6, redMat));

	Point ctPt = Point(0, 0, -5);

	hittables.add(std::make_shared<Sphere>(ctPt, 0.3, metalMat));
	//hittables.add(std::make_shared<Sphere>(Point(-0.8, 0.0, -5.7), 0.3, redMat));

	Random rand(12);
	hittables.add(std::make_shared<Sphere>(Point(1, 1, -5), 0.3,
				std::make_shared<Emissive>(10.0 * Color(1, 0.9, 0.8))));

	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			if (i == 0 && j == 0)
				continue;

			Point pt = Point(0.4 * (rand.randomDouble(0, 0.3) + i), -0.2, 0.4 * (rand.randomDouble(0, 0.3) + j)) + ctPt;
			double chooseMat = rand.randomDouble();

			if (i == 2 && j == -2)
			{
				hittables.add(std::make_shared<Sphere>(pt + Point(0, 0.2, 0), 0.3, glassMat));
				continue;
			}

			if (chooseMat < 0.2) {
				hittables.add(std::make_shared<Sphere>(pt, 0.1, glassMat));
			}
			else if (chooseMat < 0.35)
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Lambertian>(Color(rand.randomDouble(0, 1), rand.randomDouble(0, 1.0), rand.randomDouble(0, 1.0)))));
			}
			else if (chooseMat < 0.6)
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Metal>(Color(rand.randomDouble(0, 1), rand.randomDouble(0, 1.0), rand.randomDouble(0, 1.0)), 0.1)));
			}
			else
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Emissive>(2.0 * Color(rand.randomDouble(0.5, 1), rand.randomDouble(0.5, 1.0), rand.randomDouble(0.5, 1.0)))));
			}
		}
	}

	auto img = cam.render(hittables);

	if (stbi_write_jpg("test_img2.jpg", cam.imageWidth(), cam.imageHeight(), 3, img.data(), 100))
		std::cout << "Success" << std::endl;
	else 
		std::cout << "Fail" << std::endl;

	return 0;
}

int main()
{
	return raytrace();
}
