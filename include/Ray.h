#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <vector3d.h>

class Ray {

public:
    Ray() {}

    Ray(const Vector3D &a, const Vector3D &b, float ti = 0.0) {
        A = a;
        B = b;
        _time = ti;
    }

    Vector3D origin() const { return A; }

    Vector3D direction() const { return B; }

    float time() const { return _time; }

    Vector3D point_at_parameter(float t) const { return A + B * t; }

    Vector3D A;
    Vector3D B;
    float _time;
};

#endif //RAYTRACER_RAY_H
