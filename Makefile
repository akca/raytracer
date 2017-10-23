all:
	g++ *.cpp -o main -Wall -std=c++11 -O3
clean:
	rm main
run:
	./main hw1_sample_scenes/simple_shading.xml
