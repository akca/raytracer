all:
	g++ *.cpp -o main -std=c++11 -O3
clean:
	rm main
run:
	./main hw1_sample_scenes/simple.xml
