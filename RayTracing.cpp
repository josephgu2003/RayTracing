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
	params.samplesPerPixel = 150;
	params.imgWidth = 400;
	params.vFov = 30.0;
	params.pos = Point(0, 1, 0);
	params.lookAt = Point(0, 0, -5);
	Camera cam(params, Random(1));
	// SCENE
	HittableList hittables;

	auto redMat = std::make_shared<Lambertian>(Color(1, 0.5, 0) * 0.4);
	auto metalMat = std::make_shared<Metal>(Color(0.7, 1.0, 1) * 0.4, 0.0);
	auto greenMat = std::make_shared<Lambertian>(Color(0.8, 0.8, 1) * 0.4);
	auto glassMat = std::make_shared<Dielectric>(1.5);

	hittables.add(std::make_shared<Sphere>(Point(0, -1000.3, -5), 1000, greenMat));

	hittables.add(std::make_shared<Sphere>(Point(0, 0.0, -5.), 0.3, redMat));
	hittables.add(std::make_shared<Sphere>(Point(0.8, 0.0, -3.5), 0.3, metalMat));
	hittables.add(std::make_shared<Sphere>(Point(-0.8, 0.0, -6.), 0.3, glassMat));

	Random rand(3);

	for (int i = -5; i < 5; i++)
	{
		for (int j = -7; j < 3; j++)
		{
			Point pt = Point((rand.randomDouble() + i) * 0.6, -0.2, rand.randomDouble() + j);
			double chooseMat = rand.randomDouble();

			if (chooseMat < 0.2) {
				hittables.add(std::make_shared<Sphere>(pt, 0.1, glassMat));
			}
			else if (chooseMat < 0.8)
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Lambertian>(Color(rand.randomDouble(0.4, 1), rand.randomDouble(0.4, 1.0), rand.randomDouble(0.4, 1.0)))));
			}
			else
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Metal>(Color(rand.randomDouble(0.4, 1), rand.randomDouble(0.4, 1.0), rand.randomDouble(0.4, 1.0)), 0.1)));
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
