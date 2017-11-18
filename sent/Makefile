compile:
	g++ *.cpp -o raytracer -Wall -pthread -std=c++17 -O3 -march=native
all: simple simple_shading simple_reflectance cornellbox mirror_spheres bunny dragon_bg sciencetree dragon_lowres horse_and_mug
clean:
	rm raytracer *.ppm
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
