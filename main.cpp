/*
	foreach pixel s:
		compute the viewing ray r (from e to s)
		tmin = ∞, obj = NULL
		foreach object o:
			if r intersects o at point x:
				if t < tmin:
					tmin = t, obj = o
		if obj not NULL: // viewing ray intersected with an object
			pixel color = La // ambient shading is not affected by shadows
			foreach light l:
				compute the shadow ray s from x to l
				foreach object p:
					if s intersects p before the light source:
						continue the light loop; // point is in shadow – no contribution from this light
			pixel color += Ld + Ls // add diffuse and specular components for this light source
		else
			pixel color = color of background (or black)s
*/

#include <iostream>
#include <string>
#include "parser.h"
#include "ppm.h"
#include "vector3d.h"
#include "object.h"
#include "sphere.h"

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{
	// Sample usage for reading an XML scene file
	parser::Scene scene;
		
		scene.loadFromXml(argv[1]);
		
		for (parser::Camera camera : scene.cameras) {
			std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;
				
				int width = camera.image_width;
				int height = camera.image_height;
				
				unsigned char* image = new unsigned char [width * height * 3];
				
				int i = 0;
				for (int y = 0; y < height; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						for (parser::Sphere sphere : scene.spheres) {

							Vector3D cameraPosition (camera.position.x, camera.position.y, camera.position.z);
							Vector3D cameraGaze (camera.gaze.x, camera.gaze.y, camera.gaze.z);
							Vector3D cameraUp (camera.up.x, camera.up.y, camera.up.z);
							Vector3D cameraRight = cameraGaze.inverse() * cameraUp; // cross product TODO
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
							std::cout << "DIR: " << direction.x << " " << direction.y << " " << direction.z << std::endl;							
							direction.normalize();

							parser::Vec3f sphereCenterVec3f = scene.vertex_data[sphere.center_vertex_id - 1];
							
							Vector3D sphereCenter (sphereCenterVec3f.x, sphereCenterVec3f.y, sphereCenterVec3f.z);
							
							Sphere mySphere (sphereCenter, sphere.radius);
							
							//std::cout << "SPHERE: " << sphereCenter.x << " " << sphereCenter.y << " " << sphereCenter.z << " " << sphere.radius << std::endl;
							
							float t;						

							if (mySphere.intersects(cameraPosition, direction, t)) {
							Vector3D intersectionPoint = cameraPosition + direction * t;
						
								std::cout << "ASIRI DERECEDE ilginc: " << intersectionPoint.x << " "  
															<< intersectionPoint.y << " "
															<< intersectionPoint.z
															<< std::endl;
								image[i++] = 255; // r
								image[i++] = 100; // g
								image[i++] = 100; // b
							} else {
								image[i++] = 0; // r
								image[i++] = 55; // g
								image[i++] = 0; // b
							}
						}
					}
				}
			
			
			
			
			
			
			Vector3D cameraPosition (camera.position.x, camera.position.y, camera.position.z);
							
			double pixelPositionX = (camera.near_plane.y - camera.near_plane.x) * (256 + 0.5) / 1024;
			double pixelPositionY = (camera.near_plane.w - camera.near_plane.z) * (192 + 0.5) / 768;
			Vector3D pixelPosition (pixelPositionX, pixelPositionY, 
															-camera.near_distance);
			Vector3D direction = pixelPosition - cameraPosition;
			
			std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
			

			
			
			
			
			
			
			
			
			
			
				write_ppm((camera.image_name).c_str(), image, width, height);
		}
}

