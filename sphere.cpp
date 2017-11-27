#include "sphere.h"
#include <algorithm>
#include <iostream>

bool Sphere::intersects(const Vector3D &origin, const Vector3D &direction,
                        float &t, Vector3D &normal, bool isShadowRay) {

  Vector3D L = origin - center;

  float b = 2 * direction.dotProduct(L);
  float c = L.dotProduct(L) - r2;

  float t0, t1; // these will become roots
  if (!quadraticSolve(1, b, c, t0, t1)) {
    return false;
  }

  if (t0 > t1)
    std::swap(t0, t1);

  if (t1 < 0)
    return false; // both roots are negative, no intersection

  float tmin;
  if (t0 < 0) {
    tmin = t1; // if t0 is negative, t1 should be the visible param.
  } else {
    tmin = t0;
  }

  // if a closer object is already there, this object is not visible
  if (tmin > t)
    return false;
  else
    t = tmin;

  // TODO HITPOINT RECALCULATED. POSSIBLE OPTIMIZE
  normal = (L + direction * t).normalize();

  return true;
}

Vec2f Sphere::getTexturePoint(Vector3D &intersectPoint) {
  Vec2f result;
  Vector3D relative = intersectPoint - center;
  result.x = -atan2(relative.z, relative.x) / (2 * M_PI) + 0.5f; // u
  result.y = acos(relative.y / r) / M_PI;                        // v
  return result;
}

void Sphere::translation(Vector3D &t) { center = center + t; }

void Sphere::applyTransform() {
  if (transformMatrix == NULL) {
    return;
  }
  center.applyTransform(transformMatrix);
  
  delete[] transformMatrix;
  transformMatrix = NULL;
}
