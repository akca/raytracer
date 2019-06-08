#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <vector3d.h>

class Ray {

public:
    Ray() {}

    Ray(const Vector3D &ray_origin, const Vector3D &ray_direction) {
        origin = ray_origin;
        direction = ray_direction;
        time = drand48(); // TODO: check performance
    }

    Ray(const Vector3D &ray_origin, const Vector3D &ray_direction, float time) {
        origin = ray_origin;
        direction = ray_direction;
        this->time = time;
    }

    Vector3D origin;
    Vector3D direction;
    float time;
};

#endif //RAYTRACER_RAY_H
