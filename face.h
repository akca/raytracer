#ifndef FACE_H__
#define FACE_H__

#include "object.h"
#include "vector3d.h"

class Face {
public:
  Vector3D v1;
  Vector3D edge1;
  Vector3D edge2;
  Vector3D normal;
  Face() {}

  Face(Vector3D &vt1, Vector3D vt2, Vector3D vt3)
      : v1(vt1), edge1(vt2), edge2(vt3), normal((vt2 * vt3).normalize()) {}

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &tmin, bool isShadowRay) {

    Vector3D pVector = direction * edge2;
    float det = edge1.dotProduct(pVector);

    // do back-face culling if not shadow ray.
    // otherwise ignore rays that orthogonal to triangle normal.
    if ((!isShadowRay && det < 0.0f) || (isShadowRay && fabs(det) < 0.0f))
      return false;

    float inverseDet = 1 / det;
    float u, v;

    Vector3D tVector = origin - v1;
    u = inverseDet * tVector.dotProduct(pVector);
    if (u < 0.0f || u > 1.0f) {
      return false;
    }

    Vector3D qVector = tVector * edge1;
    v = inverseDet * direction.dotProduct(qVector);
    if (v < 0.0f || u + v > 1.0f) {
      return false;
    }

    float tmin_new = inverseDet * edge2.dotProduct(qVector);

    if (tmin_new > 0.0f && tmin_new < tmin) {
      tmin = tmin_new;
      return true;
    } else {
      return false;
    }
  }
};

#endif
