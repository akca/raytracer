#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "object.h"
#include "vector3d.h"
#include <iostream>
#include <cmath>

class Triangle : public Object {
public:
    Vector3D v1;
    Vector3D edge1;
    Vector3D edge2;
    Vector3D normal;
    Vector3D minPoint;
    Vector3D maxPoint;

    Vector3D vertex_normal_1;
    Vector3D vertex_normal_2;
    Vector3D vertex_normal_3;

    bool is_smooth_shading = false;
    bool bounding_box_ready = false;

    // texcoords
    Vec2f texCoord1;
    Vec2f texCoord2;
    Vec2f texCoord3;

    Triangle() = default;

    Triangle(Vector3D &vt1, Vector3D &vt2, Vector3D &vt3, int material, int texture,
             const Vec2f &texCoord1, const Vec2f &texCoord2, const Vec2f &texCoord3)
            : v1(vt1), edge1(vt2), edge2(vt3),
              texCoord1(texCoord1), texCoord2(texCoord2), texCoord3(texCoord3) {

        minPoint.e[0] = std::min(vt1.x(), std::min(vt2.x(), vt3.x()));
        minPoint.e[1] = std::min(vt1.y(), std::min(vt2.y(), vt3.y()));
        minPoint.e[2] = std::min(vt1.z(), std::min(vt2.z(), vt3.z()));

        maxPoint.e[0] = std::max(vt1.x(), std::max(vt2.x(), vt3.x()));
        maxPoint.e[1] = std::max(vt1.y(), std::max(vt2.y(), vt3.y()));
        maxPoint.e[2] = std::max(vt1.z(), std::max(vt2.z(), vt3.z()));

        edge1 = edge1 - v1;
        edge2 = edge2 - v1;
        normal = (edge1 * edge2).normalize();

        material_id = material;
        texture_id = texture;
    }

    Triangle(Vector3D &vt1, Vector3D &vt2, Vector3D &vt3, int material)
            : Triangle(vt1, vt2, vt3, material, -1, Vec2f(0, 0), Vec2f(0, 0), Vec2f(0, 0)) {

    }

    void init_bounding_box() {

        if (bounding_box_ready)
            return;

        // extend bounding box by motion_blur vector
        for (int i = 0; i < 3; i++) {
            if (motion_blur.e[i] < -0.001) {
                minPoint.e[i] += motion_blur.e[i];
            } else if (motion_blur.e[i] > 0.001) {
                maxPoint.e[i] += motion_blur.e[i];
            }
        }

        // if triangle is parallel to one of the planes,
        for (int i = 0; i < 3; i++) {
            if (std::fabs(maxPoint.e[i] - minPoint.e[i]) < 0.0001) {
                minPoint.e[i] -= 0.0001;
            }
        }

        bounding_box_ready = true;
    }

    bool bounding_box(float t0, float t1, BBox &box) override {

        init_bounding_box();

        box = BBox(minPoint, maxPoint);

        return true;
    }

    bool intersects(Ray &ray, float tmin, float tmax, HitRecord &hit_record, bool backfaceCulling) override {

        Vector3D ray_origin = ray.origin;

        if (!motion_blur.isZero()) {
            ray_origin = ray_origin - (motion_blur * ray.time);
        }

        Vector3D pVector = ray.direction * edge2;
        float det = edge1.dotProduct(pVector);

        // do back-face culling if not shadow ray.
        // otherwise ignore rays that orthogonal to triangle normal.
        if ((backfaceCulling && det < 0.0f) || (!backfaceCulling && std::fabs(det) < 0.0f))
            return false;

        float inverseDet = 1 / det;
        float u, v;

        Vector3D tVector = ray_origin - v1;
        u = inverseDet * tVector.dotProduct(pVector);
        if (u < -1e-7 || u > 1.00001) {
            return false;
        }

        Vector3D qVector = tVector * edge1;
        v = inverseDet * ray.direction.dotProduct(qVector);
        if (v < -1e-7 || u + v > 1.00001) {
            return false;
        }

        float t_new = inverseDet * edge2.dotProduct(qVector);

        if (t_new < tmax && t_new > tmin) {

            hit_record.t = t_new;

            if (is_smooth_shading) {
                hit_record.normal = vertex_normal_1 * (1 - u - v) + vertex_normal_2 * (u) + vertex_normal_3 * (v);
            } else {
                hit_record.normal = normal;
            }

            hit_record.material_id = material_id;
            hit_record.texture_id = texture_id;

            if (texture_id != -1) {
                hit_record.texture_coords = {texCoord1.x + u * (texCoord2.x - texCoord1.x) +
                                             v * (texCoord3.x - texCoord1.x),
                                             texCoord1.y + u * (texCoord2.y - texCoord1.y) +
                                             v * (texCoord3.y - texCoord1.y)};
            }

            return true;

        } else {
            return false;
        }
    }
};

#endif
