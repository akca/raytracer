#ifndef SPHERE_H__
#define SPHERE_H__

#include "object.h"
#include "vector3d.h"
#include <algorithm>

class Sphere : public Object {
public:
    Vector3D center; // center of the sphere
    float r;
    float r2;
    float *transformMatrix = nullptr;
    float *invTransformMatrix = nullptr;
    float invTransposeTransformMatrix[16]{};

    Sphere(Vector3D &c, float radius, int m, int t)
            : center(c), r(radius), r2(r * r) {
        material_id = m;
        texture_id = t;
    }

    bool bounding_box(float t0, float t1, BBox &box) override {
        box = BBox(center - Vector3D(r, r, r), center + Vector3D(r, r, r));
        return true;
    }

    bool intersects(const Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) override {

        Vector3D newOrigin = ray.origin;
        Vector3D newDirection = ray.direction;

        if (invTransformMatrix) {
            newDirection.applyTransform(invTransformMatrix, true);
            newDirection.normalize();
            newOrigin.applyTransform(invTransformMatrix, false);
        }

        Vector3D L = newOrigin - center;

        float b = 2 * newDirection.dotProduct(L);
        float c = L.dotProduct(L) - r2;

        float t0, t1; // these will become roots
        if (!quadraticSolve(1, b, c, t0, t1)) {
            return false;
        }

        if (t0 > t1)
            std::swap(t0, t1);

        if (t1 < 0)
            return false; // both roots are negative, no intersection

        if (t0 < 0) {
            t0 = t1; // if t0 is negative, t1 should be the visible param.
        }

        // if a closer object is already there, this object is not visible
        if (tmin > t0 || t0 > tmax)
            return false;
        else {
            hit_record.t = t0;
            hit_record.intersection_point = newOrigin + newDirection * hit_record.t;
            hit_record.normal = (hit_record.intersection_point - center).normalize();
            hit_record.material_id = material_id;
            hit_record.texture_id = texture_id;

            if (texture_id != -1) {
                Vector3D relative = hit_record.intersection_point - center;
                hit_record.texture_coords.x = static_cast<float>(-atan2f(relative.z(), relative.x())
                                                                 / (2 * M_PI) + 0.5f); // u
                hit_record.texture_coords.y = static_cast<float>(acosf(relative.y()
                                                                       / r) / M_PI);   // v
            }

            if (transformMatrix) {
                hit_record.intersection_point.applyTransform(transformMatrix, false);
                hit_record.normal.applyTransform(invTransposeTransformMatrix, true);
                hit_record.normal.normalize();
            }

            return true;
        }
    }
};

#endif
