compile:
	g++ *.cpp -o raytracer -Wall -pthread -ljpeg -std=c++17 -O3 -march=native
debug:
	g++ *.cpp -o raytracer -Wall -pthread -ljpeg -std=c++17 -g
clean:
	rm raytracer *.ppm

all: hw1 hw2

hw1: simple simple_shading simple_reflectance cornellbox mirror_spheres bunny dragon_bg sciencetree sciencetree_wp dragon_lowres horse_and_mug
hw2: simple_transform skybox spheres_transform ellipsoids_texture_bilinear ellipsoids_texture_nearest horse_instanced horse

simple:
	./raytracer hw1_sample_scenes/simple.xml
simple_shading:
	./raytracer hw1_sample_scenes/simple_shading.xml
simple_reflectance:
	./raytracer hw1_sample_scenes/simple_reflectance.xml
cornellbox:
	./raytracer hw1_sample_scenes/cornellbox.xml
mirror_spheres:
	./raytracer hw1_sample_scenes/mirror_spheres.xml
bunny:
	./raytracer hw1_sample_scenes/bunny.xml
dragon_lowres:
	./raytracer hw1_sample_scenes/dragon_lowres.xml
dragon_bg:
	./raytracer hw1_sample_scenes/dragon_bg.xml
horse_and_mug:
	./raytracer hw1_sample_scenes/horse_and_mug.xml
sciencetree:
	./raytracer hw1_sample_scenes/sciencetree.xml
sciencetree_wp:
	./raytracer hw1_sample_scenes/sciencetree_wp.xml

# hw2
ellipsoids_texture_bilinear:
	./raytracer hw2_sample_scenes/ellipsoids_texture_bilinear.xml
ellipsoids_texture_nearest:
	./raytracer hw2_sample_scenes/ellipsoids_texture_nearest.xml
horse_instanced:
	./raytracer hw2_sample_scenes/horse_instanced.xml
horse:
	./raytracer hw2_sample_scenes/horse.xml
simple_transform:
	./raytracer hw2_sample_scenes/simple_transform.xml
skybox:
	./raytracer hw2_sample_scenes/skybox.xml
spheres_transform:
	./raytracer hw2_sample_scenes/spheres_transform.xml
