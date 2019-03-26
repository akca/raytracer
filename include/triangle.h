#include <cmath>

#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "object.h"
#include "vector3d.h"

class Triangle : public Object {
public:
    Vector3D v1;
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

    Triangle(Vector3D &vt1, Vector3D vt2, Vector3D vt3, int material, int texture,
             Vec2f &texCoord1, Vec2f &texCoord2, Vec2f &texCoord3)
            : v1(vt1), edge1(vt2), edge2(vt3), normal((vt2 * vt3).normalize()),
              texCoord1(texCoord1), texCoord2(texCoord2), texCoord3(texCoord3) {

        minPoint.e[0] = std::min(vt1.x(), std::min(vt2.x(), vt3.x()));
        minPoint.e[1] = std::min(vt1.y(), std::min(vt2.y(), vt3.y()));
        minPoint.e[2] = std::min(vt1.z(), std::min(vt2.z(), vt3.z()));

        maxPoint.e[0] = std::max(vt1.x(), std::max(vt2.x(), vt3.x()));
        maxPoint.e[1] = std::max(vt1.y(), std::max(vt2.y(), vt3.y()));
        maxPoint.e[2] = std::max(vt1.z(), std::max(vt2.z(), vt3.z()));

        material_id = material;
        texture_id = texture;
    }

    BBox boundingBox(float x, float y) override {

        return BBox(minPoint, maxPoint);
    }

    bool intersects(const Ray &ray, float &tmin, HitRecord &hit_record, bool backfaceCulling) override {

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

        float tmin_new = inverseDet * edge2.dotProduct(qVector);

        if (tmin_new > 0.0f && tmin_new < tmin) {
            tmin = tmin_new;

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
