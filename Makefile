all:
	g++ -pthread *.cpp -o main -Wall -std=c++11 -O3
clean:
	rm main *.ppm
simple:
	./main hw1_sample_scenes/simple.xml
	eog simple.ppm &
simple_shading:
	./main hw1_sample_scenes/simple_shading.xml
	eog simple_shading.ppm &
simple_reflectance:
	./main hw1_sample_scenes/simple_reflectance.xml
	eog simple_reflectance.ppm &
cornellbox:
	./main hw1_sample_scenes/cornellbox.xml
mirror_spheres:
	./main hw1_sample_scenes/mirror_spheres.xml
	eog mirror_spheres.ppm &
bunny:
	./main hw1_sample_scenes/bunny.xml
	eog bunny.ppm &
dragon_lowres:
	./main hw1_sample_scenes/dragon_lowres.xml
	eog dragon_lowres.ppm &
dragon_bg:
	./main hw1_sample_scenes/dragon_bg.xml
	eog dragon_bg.ppm &
horse_and_mug:
	./main hw1_sample_scenes/horse_and_mug.xml
	eog horse_and_mug.ppm &
sciencetree:
	./main hw1_sample_scenes/sciencetree.xml
	eog scienceTree_1080x1080.ppm &
