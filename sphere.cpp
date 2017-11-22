#include "sphere.h"
#include <algorithm>

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

  if (t0 < 0) {
    t = t1; // if t0 is negative, t1 should be the visible param.
  } else {
    t = t0;
  }

  // TODO HITPOINT RECALCULATED. POSSIBLE OPTIMIZE
  normal = (L + direction * t).normalize();

  return true;
}
