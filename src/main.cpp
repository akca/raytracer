#include <iostream>
#include <thread>
#include <shade.h>
#include <ppm.h>

int main(int argc, char *argv[]) {
    parser::Scene scene;

    scene.loadFromXml(argv[1]);
    using std::cout;
    using std::endl;
    using std::thread;

    for (parser::Camera camera : scene.cameras) {
        std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;

        int width = camera.image_width;
        int height = camera.image_height;

        auto *image = new unsigned char[width * height * 3];

        size_t threadCount = 32;
        int partition = height / threadCount;
        int startHeight = 0;
        int endHeight = partition;
        std::vector<thread> threads;

        for (size_t i = 0; i < threadCount; i++) {
            threads.emplace_back(trace, &scene, &camera, startHeight, endHeight,
                                 width, height, image);
            startHeight = endHeight;
            if (i >= threadCount - 2) {
                endHeight = height;
            } else {
                endHeight += partition;
            }
        }

        for (std::thread &t1 : threads) {
            t1.join();
        }

        threads.clear();

        write_ppm((camera.image_name).c_str(), image, width, height);
        delete[] image;
    }

    // deallocate objects
    for (auto &obj : scene.objects) {
        delete obj;
    }
}