#include "triangle.h"

bool Triangle::intersects(const Vector3D &origin, const Vector3D &direction,
                          float &tmin, Vector3D &intersectPoint,
                          Vector3D &norml, bool isShadowRay, Vec2f &texCoord) {

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
  if (u < -1e-7 || u > 1.00001) {
    return false;
  }

  Vector3D qVector = tVector * edge1;
  v = inverseDet * direction.dotProduct(qVector);
  if (v < -1e-7 || u + v > 1.00001) {
    return false;
  }

  float tmin_new = inverseDet * edge2.dotProduct(qVector);

  if (tmin_new > 0.0f && tmin_new < tmin) {
    tmin = tmin_new;

    if (!isShadowRay) {
      norml = normal;
      if (texture_id != -1) {
        texCoord = {texCoord1.x + u * (texCoord2.x - texCoord1.x) +
                        v * (texCoord3.x - texCoord1.x),
                    texCoord1.y + u * (texCoord2.y - texCoord1.y) +
                        v * (texCoord3.y - texCoord1.y)};
      }
    }

    return true;
  } else {
    return false;
  }
}
