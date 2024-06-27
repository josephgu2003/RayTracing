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
#include "Quad.h"

using namespace std;

using MatPtr = std::shared_ptr<Material>;

void placeBox(HittableList& hittables, Point origin, double x, double y, double z, 
	MatPtr bot, MatPtr top, MatPtr left, MatPtr right, MatPtr front, MatPtr back)
{
	hittables.add(std::make_shared<Quad>(origin + Point(-x, -y, -z), Vec(2 * x, 0, 0), Vec(0, 2 * y, 0), back));
	hittables.add(std::make_shared<Quad>(origin + Point(-x, -y, z), Vec(2 * x, 0, 0), Vec(0, 2 * y, 0), front));

	hittables.add(std::make_shared<Quad>(origin + Point(-x, -y, -z), Vec(0, 2 * y, 0), Vec(0, 0, 2 * z), left));
	hittables.add(std::make_shared<Quad>(origin + Point(x, -y, -z), Vec(0, 2 * y, 0), Vec(0, 0, 2 * z), right));

	hittables.add(std::make_shared<Quad>(origin + Point(-x, -y, -z), Vec(2 * x, 0, 0), Vec(0, 0, 2 * z), bot));
	hittables.add(std::make_shared<Quad>(origin + Point(-x, y, -z), Vec(2 * x, 0, 0), Vec(0, 0, 2 * z), top));
}

int cornellBox()
{
	CamParams params;
	params.samplesPerPixel = 200;
	params.imgWidth = 300;
	params.vFov = 50.0;
	params.pos = Point(0, 0.0, 2);
	params.lookAt = Point(0, 0, -5);
	params.defocusAngle = -1;
	params.maxDepth = 8;
	params.aspectRatio = 1.0;
	Camera cam(params, Random(1));
	// SCENE
	HittableList hittables;

	auto redMat = std::make_shared<Lambertian>(Color(1, 0, 0));
	auto metalMat = std::make_shared<Metal>(Color(0.7, 1.0, 1), 0.0);
	auto whiteMat = std::make_shared<Lambertian>(Color(1, 1, 1));
	auto greenMat = std::make_shared<Lambertian>(Color(0, 1, 0));
	auto glassMat = std::make_shared<Dielectric>(1.5);

	auto lightMat = std::make_shared<Emissive>(2.0 * Color(1, 0.9, 0.8));

	Random rand(100);

	auto color1 = std::make_shared<Emissive>(10.0 * Color(0.3, 0.9, 0.4));
	auto color2 = std::make_shared<Emissive>(10.0 * Color(0.3, 0.3, 0.9));
	auto color3 = std::make_shared<Emissive>(10.0 * Color(0.9, 0.3, 0.3));

	std::shared_ptr<Emissive> colors[] = {color1, color2, color3};
	// environment

	hittables.add(std::make_shared<Quad>(Point(-0.5, 1.95, -5), Vec(1, 0, 0), Vec(0, 0, 1), lightMat));
	
	placeBox(hittables, Point(0, 0, 0), 2, 2, 6, whiteMat, whiteMat, greenMat, redMat, whiteMat, whiteMat);

	placeBox(hittables, Point(-0.5, -1, -5), 0.3, 1.5, 0.3, whiteMat, whiteMat, whiteMat, whiteMat, whiteMat, whiteMat);
	placeBox(hittables, Point(0.5, -1, -3.5), 0.3, 1, 0.3, whiteMat, whiteMat, whiteMat, whiteMat, whiteMat, whiteMat);

	auto img = cam.render(hittables);

	if (stbi_write_jpg("test_img2.jpg", cam.imageWidth(), cam.imageHeight(), 3, img.data(), 100))
		std::cout << "Success" << std::endl;
	else 
		std::cout << "Fail" << std::endl;

	return 0;
}


int raytrace()
{
	CamParams params;
	params.samplesPerPixel = 2000;
	params.imgWidth = 300;
	params.vFov = 20.0;
	params.pos = Point(0, 2, 0);
	params.lookAt = Point(0, 0, -5);
	params.defocusAngle = -1;
	params.maxDepth = 8;
	Camera cam(params, Random(1));
	// SCENE
	HittableList hittables;

	auto redMat = std::make_shared<Lambertian>(Color(1, 1, 1) * 0.8);
	auto metalMat = std::make_shared<Metal>(Color(0.7, 1.0, 1), 0.0);
	auto whiteMat = std::make_shared<Lambertian>(Color(1, 1.0, 1));
	auto greenMat = std::make_shared<Lambertian>(Color(0.8, 0.8, 1) * 0.4);
	auto glassMat = std::make_shared<Dielectric>(1.5);

	// environment
	hittables.add(std::make_shared<Sphere>(Point(0, -1000.3, -5), 1000, greenMat));
	hittables.add(std::make_shared<Sphere>(Point(0, 0, 0), 6, redMat));
	hittables.add(std::make_shared<Sphere>(Point(0, 3, 0), 1,
				std::make_shared<Emissive>(0.5 * Color(1, 0.9, 0.8))));

	Point ctPt = Point(0, 0, -5);

	hittables.add(std::make_shared<Sphere>(ctPt, 0.3, metalMat));
	//hittables.add(std::make_shared<Sphere>(Point(-0.8, 0.0, -5.7), 0.3, redMat));

	Random rand(100);

	auto color1 = std::make_shared<Emissive>(10.0 * Color(0.3, 0.9, 0.4));
	auto color2 = std::make_shared<Emissive>(10.0 * Color(0.3, 0.3, 0.9));
	auto color3 = std::make_shared<Emissive>(10.0 * Color(0.9, 0.3, 0.3));

	std::shared_ptr<Emissive> colors[] = {color1, color2, color3};

	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			if (i == 0 && j == 0)
				continue;

			Point pt = Point(0.6 * (rand.randomDouble(0, 0.7) + i), -0.2, 0.6 * (rand.randomDouble(0, 0.7) + j)) + ctPt;
			double chooseMat = rand.randomDouble();

			if (i == 2 && j == 2)
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
			else if (chooseMat < 0.5)
			{
				hittables.add(std::make_shared<Sphere>(pt, 0.1,
					std::make_shared<Metal>(Color(rand.randomDouble(0, 1), rand.randomDouble(0, 1.0), rand.randomDouble(0, 1.0)), 0.1)));
			}
			else if (chooseMat < 0.6)
			{
				hittables.add(std::make_shared<Sphere>(pt + Point(0, 0.1, 0), 0.2, colors[(int)rand.randomDouble(0, 3)]));
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
	return cornellBox();
}
