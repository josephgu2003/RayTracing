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
/**
template <typename T>
Color rayColor(const Ray& ray, T hittables)
{
	Interval interval(-100, 100);
	Hit hit;
	if (hittables.hit(ray, interval, hit))
	{
		return (hit.normal + 1.0) * 0.5;
	}

	auto unit_direction = ray.dir();
	auto a = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}
*/

int raytrace()
{
	Camera cam(16.0 / 9.0, 0.1, 550, 20);
	// SCENE
	HittableList hittables;

	std::shared_ptr<std::mt19937> generator = std::make_shared<std::mt19937>();
	auto redMat = std::make_shared<Lambertian>(Color(1, 0.5, 0) * 0.4, generator);
	auto metalMat = std::make_shared<Metal>(Color(0.7, 1.0, 1) * 0.4, 0.3, generator);
	auto greenMat = std::make_shared<Lambertian>(Color(0.8, 0.8, 1) * 0.4, generator);
	auto glassMat = std::make_shared<Dielectric>(1.5);
	auto bubbleMat = std::make_shared<Dielectric>(1.0 / 1.5);

	hittables.add(std::make_shared<Sphere>(Point(0, 0.0, -1.5), 0.3, redMat));
	hittables.add(std::make_shared<Sphere>(Point(0.8, 0.0, -1.5), 0.3, metalMat));
	hittables.add(std::make_shared<Sphere>(Point(-0.8, 0.0, -1.5), 0.3, glassMat));
	hittables.add(std::make_shared<Sphere>(Point(-0.8, 0.0, -1.5), 0.28, bubbleMat));
	hittables.add(std::make_shared<Sphere>(Point(0, -100.3, -1.5), 100, greenMat));

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
