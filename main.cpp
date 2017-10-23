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

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{
	// Sample usage for reading an XML scene file
	parser::Scene scene;

	scene.loadFromXml(argv[1]);
	
	for (parser::Camera camera : scene.cameras) {
		std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;
	
		// The code below creates a test pattern and writes
		// it to a PPM file to demonstrate the usage of the
		// ppm_write function.

		const RGB BAR_COLOR[8] =
		{
			{ 255, 255, 255 },  // 100% White
			{ 255, 255,   0 },  // Yellow
			{   0, 255, 255 },  // Cyan
			{   0, 255,   0 },  // Green
			{ 255,   0, 255 },  // Magenta
			{ 255,   0,   0 },  // Red
			{   0,   0, 255 },  // Blue
			{   0,   0,   0 },  // Black
		};

		int width = camera.image_width, height = camera.image_height;
		int columnWidth = width / 8;

		unsigned char* image = new unsigned char [width * height * 3];

		int i = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int colIdx = x / columnWidth;
				image[i++] = BAR_COLOR[colIdx][0];
				image[i++] = BAR_COLOR[colIdx][1];
				image[i++] = BAR_COLOR[colIdx][2];
			}
		}

		write_ppm((camera.image_name).c_str(), image, width, height);
	}
}

