#include "triangle.h"

bool Triangle::intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                Vector3D &normal, bool isShadowRay) {
  if (face.intersects(origin, direction, t, isShadowRay)) {
    normal = face.normal;
    return true;
  }
  return false;
}
