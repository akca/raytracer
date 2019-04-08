#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <vector3d.h>

class Ray {

public:
    Ray() {}

    Ray(const Vector3D &ray_origin, const Vector3D &ray_direction) {
        origin = ray_origin;
        direction = ray_direction;
    }

    Vector3D origin;
    Vector3D direction;
};

#endif //RAYTRACER_RAY_H
