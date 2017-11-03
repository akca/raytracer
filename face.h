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

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &tmin,
                  bool isShadowRay) {
    float u;
    float v;

    Vector3D v1v2 = v2 - v1;
    Vector3D v1v3 = v3 - v1;
    Vector3D pvec = direction * v1v3;
    float det = v1v2.dotProduct(pvec);

    // do back-face culling if not shadow ray.
    // otherwise ignore rays that parallel to triangle normal.



    // TODO MAKE IT BETTER !?!?!?!



    if ((!isShadowRay && det < 1e-8) || (fabs(det) < 1e-8))
      return false;

    float invDet = 1 / det;

    Vector3D tvec = origin - v1;
    u = tvec.dotProduct(pvec) * invDet;
    if (u < 0 || u > 1) return false;

    Vector3D qvec = tvec * v1v2;
    v = direction.dotProduct(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    float tmin_new = v1v3.dotProduct(qvec) * invDet;
    if (tmin_new < tmin) {
      tmin = tmin_new;
      return true;
    } else {
      return false;
    }
  }
};

#endif
