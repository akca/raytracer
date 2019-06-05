#ifndef MESH_H__
#define MESH_H__

#include "object.h"
#include "vector3d.h"
#include <iostream>
#include <cmath>

class Mesh : public Object {
public:
    std::vector<Triangle *> faces;

    Mesh() = default;

    bool bounding_box(float t0, float t1, BBox &box) override {

        if (faces.empty()) return false;

        BBox temp_box;

        bool first_true = faces[0]->bounding_box(t0, t1, temp_box);
        if (!first_true)
            return false;
        else
            box = temp_box;

        for (size_t i = 1; i < faces.size(); i++) {
            // TODO shirley's bug
            if (faces[i]->bounding_box(t0, t1, temp_box)) {
                box = surround(box, temp_box);
            } else
                return false;
        }
        return true;
    }

    bool intersects(Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) override {

        HitRecord temp_rec;
        bool hit_anything = false;
        float closest_so_far = tmax;

        for (auto &face : faces) {
            if (face->intersects(ray, tmin, closest_so_far, temp_rec, backfaceCulling)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                hit_record = temp_rec;
            }
        }
        return hit_anything;
    }
};

#endif
