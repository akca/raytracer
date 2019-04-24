#ifndef RAYTRACER_VERTEX_H
#define RAYTRACER_VERTEX_H

#include "vector3d.h"

class Vertex : public Vector3D {
public:
    Vector3D normal;

    Vertex() = default;

    Vertex(const Vec3f &v) {
        e[0] = v.x;
        e[1] = v.y;
        e[2] = v.z;
    }

    void addNormal(Vector3D &n) {
        normal = normal + n;
    }

    void normalizeNormal() {
        normal.normalize();
    }
};


#endif //RAYTRACER_VERTEX_H
