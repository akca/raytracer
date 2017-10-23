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
							
							double pixelPositionX = (camera.near_plane.y - camera.near_plane.x) * (x + 0.5) / width;
							double pixelPositionY = (camera.near_plane.w - camera.near_plane.z) * (y + 0.5) / height;
							Vector3D pixelPosition (pixelPositionX, pixelPositionY, 
																			-camera.near_distance);
							Vector3D direction = pixelPosition - cameraPosition;
							direction.normalize();
							parser::Vec3f sphereCenterVec3f = scene.vertex_data[sphere.center_vertex_id];
							
							Vector3D sphereCenter (sphereCenterVec3f.x, sphereCenterVec3f.y, sphereCenterVec3f.z);
							
							Sphere mySphere (sphereCenter, sphere.radius);
							
							if (mySphere.intersects(cameraPosition, direction) > 0) {
								
								image[i++] = 125; // r
								image[i++] = 155; // g
								image[i++] = 100; // b
							} else {
								image[i++] = 0; // r
								image[i++] = 75; // g
								image[i++] = 0; // b
							}
						}
					}
				}
			
				write_ppm((camera.image_name).c_str(), image, width, height);
		}
}

