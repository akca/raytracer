#ifndef FACE_H__
#define FACE_H__

#include "object.h"
#include "vector3d.h"

class Face {
public:
  Vector3D v1;
  Vector3D v2;
  Vector3D v3;
  Vector3D normal;
  Face() {}

  Face(Vector3D &vt1, Vector3D &vt2, Vector3D &vt3) {
    v1 = vt1;
    v2 = vt2;
    v3 = vt3;
    normal = ((v3 - v2) * (v1 - v2)).normalize();
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &tmin, bool isShadowRay) {

    Vector3D v1v2 = v2 - v1;
    Vector3D v1v3 = v3 - v1;
    Vector3D pVector = direction * v1v3;
    float det = v1v2.dotProduct(pVector);

    // do back-face culling if not shadow ray.
    // otherwise ignore rays that orthogonal to triangle normal.
    if ((!isShadowRay && det < 1e-8) || (isShadowRay && fabs(det) < 1e-8))
      return false;

    float inverseDet = 1 / det;
    float u, v;

    Vector3D tVector = origin - v1;
    u = inverseDet * tVector.dotProduct(pVector);
    if (u < -1e-7 || u > 1 + 1e-7) {
      return false;
    }

    Vector3D qVector = tVector * v1v2;
    v = inverseDet * direction.dotProduct(qVector);
    if (v < -1e-7 || u + v > 1 + 1e-7) {
      return false;
    }

    float tmin_new = inverseDet * v1v3.dotProduct(qVector);

    if (tmin_new > -1e-8 && tmin_new < tmin) {
      tmin = tmin_new;
      return true;
    } else {
      return false;
    }
  }
};

#endif
