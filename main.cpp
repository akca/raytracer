#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "parser.h"
#include "ppm.h"

const float FLOAT_MAX = std::numeric_limits<float>::max();

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{
	parser::Scene scene;

	scene.loadFromXml(argv[1]);

	for (parser::Camera camera : scene.cameras) {
		std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;

		int width = camera.image_width;
		int height = camera.image_height;

		unsigned char* image = new unsigned char [width * height * 3];

		/*		std::vector<Object> objects;

				for (parser::Sphere &sphere : scene.spheres) {
				objects.push_back(sphere);
				}
		 */
		int i = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{

				//std::cout << x << " " << y << std::endl;

				Vector3D cameraPosition (camera.position.x, camera.position.y, camera.position.z);
				Vector3D cameraGaze (camera.gaze.x, camera.gaze.y, camera.gaze.z);
				Vector3D cameraUp (camera.up.x, camera.up.y, camera.up.z);
				Vector3D cameraRight = cameraGaze * cameraUp; // cross product TODO
				//std::cout << "cameraRight: " << cameraRight.x << " " << cameraRight.y << " " << cameraRight.z << std::endl;
				float pixelPositionX = (camera.near_plane.y - camera.near_plane.x) * (x + 0.5) / width; // su
				float pixelPositionY = (camera.near_plane.w - camera.near_plane.z) * (y + 0.5) / height; // sv
				//std::cout << "PIXEL: " << pixelPositionX << " " << pixelPositionY << std::endl;
				Vector3D centerOfPlane = cameraPosition + cameraGaze * camera.near_distance;
				//std::cout << "centerOfPlane: " << centerOfPlane.x << " " << centerOfPlane.y << " " << centerOfPlane.z << std::endl;
				Vector3D planeStartPoint = centerOfPlane + (cameraRight * camera.near_plane.x) + (cameraUp * camera.near_plane.w);
				//std::cout << "planeStartPoint: " << planeStartPoint.x << " " << planeStartPoint.y << " " << planeStartPoint.z << std::endl;

				Vector3D pixelPosition = planeStartPoint + (cameraRight * pixelPositionX) - (cameraUp * pixelPositionY); // s(i,j) WRONG!! TODO
				//std::cout << "pixelPosition: " << pixelPosition.x << " " << pixelPosition.y << " " << planeStartPoint.z << std::endl;
				Vector3D direction = pixelPosition - cameraPosition;
				//std::cout << "DIR: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
				direction.normalize();

				Object* nearObject = scene.objects[0];
				float tmin = FLOAT_MAX;

				for (auto &object : scene.objects) {

					float t = FLOAT_MAX;

					if (object->intersects(cameraPosition, direction, t) && t<tmin) {
						tmin = t;
						nearObject = object;
					}
				}


				if (tmin < FLOAT_MAX) {
					//Vector3D intersectionPoint = cameraPosition + direction * t;

					//								std::cout << "ASIRI DERECEDE ilginc: " << intersectionPoint.x << " "
					//															<< intersectionPoint.y << " "
					//															<< intersectionPoint.z
					//															<< std::endl;
					image[i++] = 100; // r
					image[i++] = 30; // g
					image[i++] = 255; // b
				} else {
					image[i++] = scene.background_color.x; // r
					image[i++] = scene.background_color.y; // g
					image[i++] = scene.background_color.z; // b
				}


			}
		}

		write_ppm((camera.image_name).c_str(), image, width, height);
	}
}

