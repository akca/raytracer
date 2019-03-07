cmake_minimum_required(VERSION 3.13)
project(raytracer)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_STANDARD 17)


include_directories(include)

find_package(Threads)
find_package(JPEG)

add_executable(
        raytracer
        src/main.cpp
        src/parser.cpp
        src/ppm.cpp
        src/jpeg.cpp
        src/utility.cpp
        src/tinyxml2.cpp)

target_link_libraries(raytracer ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(raytracer ${JPEG_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hw1_sample_scenes
        ${CMAKE_CURRENT_SOURCE_DIR}/hw2_sample_scenes
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
