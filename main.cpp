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

typedef unsigned char RGB[3];

double dotProduct(Vector3D v1, Vector3D v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

int main(int argc, char* argv[])
{
	// Sample usage for reading an XML scene file
	parser::Scene scene;
		
		scene.loadFromXml(argv[1]);
		
		for (parser::Camera camera : scene.cameras) {
			std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;
				
				int width = camera.image_width
				int height = camera.image_height;
				
				unsigned char* image = new unsigned char [width * height * 3];
				
				int i = 0;
				for (int y = 0; y < height; ++y)
				{
					for (int x = 0; x < width; ++x)
					{
						image[i++] = 0; // r
						image[i++] = 0; // g
						image[i++] = 0; // b
					}
				}
			
				write_ppm((camera.image_name).c_str(), image, width, height);
		}
}

