all:
	g++ *.cpp -o main -Wall -std=c++11 -O3
clean:
	rm main
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
