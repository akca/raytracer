#ifndef RAYTRACER_BVH_H
#define RAYTRACER_BVH_H

#include <iostream>
#include "object.h"

class BVH : public Object {
public:
    BVH() {}

    BVH(Object **surfaces, size_t num_surfaces);

    ~BVH() {

        delete left;
        if (right != left) {
            delete right;
        }

        left = nullptr;
        right = nullptr;
    }

    bool intersects(const Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) override;

    bool bounding_box(float t0, float t1, BBox &box) override;

    BBox bbox;
    Object *left;
    Object *right;
};

#endif //RAYTRACER_BVH_H
