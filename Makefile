compile:
	g++ *.cpp -o main -Wall -pthread -std=c++17 -O3 -march=native
all: simple simple_shading simple_reflectance cornellbox mirror_spheres bunny dragon_bg sciencetree dragon_lowres horse_and_mug
clean:
	rm main *.ppm
simple:
	./main hw1_sample_scenes/simple.xml
simple_shading:
	./main hw1_sample_scenes/simple_shading.xml
simple_reflectance:
	./main hw1_sample_scenes/simple_reflectance.xml
cornellbox:
	./main hw1_sample_scenes/cornellbox.xml
mirror_spheres:
	./main hw1_sample_scenes/mirror_spheres.xml
bunny:
	./main hw1_sample_scenes/bunny.xml
dragon_lowres:
	./main hw1_sample_scenes/dragon_lowres.xml
dragon_bg:
	./main hw1_sample_scenes/dragon_bg.xml
horse_and_mug:
	./main hw1_sample_scenes/horse_and_mug.xml
sciencetree:
	./main hw1_sample_scenes/sciencetree.xml
