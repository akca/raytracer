#ifndef TRIANGLE_H__

#define TRIANGLE_H__

#include "object.h"
#include "vector3d.h"
#include <iostream>
#include <cmath>

class Triangle : public Object {
public:
    Vector3D v1;
    Vector3D v2;
    Vector3D v3;
    Vector3D edge1;
    Vector3D edge2;
    Vector3D normal;
    Vector3D minPoint;
    Vector3D maxPoint;

    // texcoords
    Vec2f texCoord1;
    Vec2f texCoord2;
    Vec2f texCoord3;

    Triangle() {}

    Triangle(Vector3D &vt1, Vector3D &vt2, Vector3D &vt3, int material, int texture,
             Vec2f &texCoord1, Vec2f &texCoord2, Vec2f &texCoord3)
            : v1(vt1), v2(vt2), v3(vt3),
              texCoord1(texCoord1), texCoord2(texCoord2), texCoord3(texCoord3) {

        edge1 = v2 - v1;
        edge2 = v3 - v1;
        normal = (edge1 * edge2).normalize();

        minPoint.e[0] = std::min(vt1.x(), std::min(vt2.x(), vt3.x()));
        minPoint.e[1] = std::min(vt1.y(), std::min(vt2.y(), vt3.y()));
        minPoint.e[2] = std::min(vt1.z(), std::min(vt2.z(), vt3.z()));

        maxPoint.e[0] = std::max(vt1.x(), std::max(vt2.x(), vt3.x()));
        maxPoint.e[1] = std::max(vt1.y(), std::max(vt2.y(), vt3.y()));
        maxPoint.e[2] = std::max(vt1.z(), std::max(vt2.z(), vt3.z()));

        material_id = material;
        texture_id = texture;
    }

    bool bounding_box(float t0, float t1, BBox &box) override {

        Vector3D bbox_min_point = minPoint;

        // if triangle is parallel to one of the planes,
        for (int i = 0; i < 3; i++) {
            if (std::fabs(maxPoint.e[i] - bbox_min_point.e[i]) < 0.00001) {
                bbox_min_point.e[i] -= 0.00001;
            }
        }

        box = BBox(bbox_min_point, maxPoint);

        return true;
    }

    bool intersects(const Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) override {

        Vector3D pVector = ray.direction() * edge2;
        float det = edge1.dotProduct(pVector);

        // do back-face culling if not shadow ray.
        // otherwise ignore rays that orthogonal to triangle normal.
        if ((backfaceCulling && det < 0.0f) || (!backfaceCulling && std::fabs(det) < 0.0f))
            return false;

        float inverseDet = 1 / det;
        float u, v;

        Vector3D tVector = ray.origin() - v1;
        u = inverseDet * tVector.dotProduct(pVector);
        if (u < -1e-7 || u > 1.00001) {
            return false;
        }

        Vector3D qVector = tVector * edge1;
        v = inverseDet * ray.direction().dotProduct(qVector);
        if (v < -1e-7 || u + v > 1.00001) {
            return false;
        }

        float t_new = inverseDet * edge2.dotProduct(qVector);

        if (t_new < tmax && t_new > tmin && t_new < hit_record.t) {
            hit_record.t = t_new;

            hit_record.normal = normal;
            if (texture_id != -1) {
                hit_record.texture_coords = {texCoord1.x + u * (texCoord2.x - texCoord1.x) +
                                             v * (texCoord3.x - texCoord1.x),
                                             texCoord1.y + u * (texCoord2.y - texCoord1.y) +
                                             v * (texCoord3.y - texCoord1.y)};
            }

            hit_record.material_id = material_id;
            hit_record.texture_id = texture_id;

            return true;
        } else {
            return false;
        }
    }
};

#endif
