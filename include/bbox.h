#ifndef RAYTRACER_BBOX_H
#define RAYTRACER_BBOX_H

#include "vector3d.h"
#include "Ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }

inline float ffmax(float a, float b) { return a > b ? a : b; }

class BBox {
public:
    BBox() {}

    BBox(const Vector3D &a, const Vector3D &b) {
        pp[0] = a;
        pp[1] = b;
    }

    Vector3D min() const { return pp[0]; }

    Vector3D max() const { return pp[1]; }

    Vector3D pp[2];

    bool rayIntersect(const Ray &r, float tmin, float tmax) const {

        for (int a = 0; a < 3; a++) {
            float t0 = ffmin((min()[a] - r.origin()[a]) / r.direction()[a],
                             (max()[a] - r.origin()[a]) / r.direction()[a]);
            float t1 = ffmax((min()[a] - r.origin()[a]) / r.direction()[a],
                             (max()[a] - r.origin()[a]) / r.direction()[a]);
            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

};

inline BBox surround(const BBox &b1, const BBox &b2) {


    return BBox(
            Vector3D(b1.min().x() < b2.min().x() ? b1.min().x() : b2.min().x(),
                     b1.min().y() < b2.min().y() ? b1.min().y() : b2.min().y(),
                     b1.min().z() < b2.min().z() ? b1.min().z() : b2.min().z()),
            Vector3D(b1.min().x() > b2.min().x() ? b1.min().x() : b2.min().x(),
                     b1.min().y() > b2.min().y() ? b1.min().y() : b2.min().y(),
                     b1.min().z() > b2.min().z() ? b1.min().z() : b2.min().z()));
}

#endif //RAYTRACER_BBOX_H
