#ifndef SPHERE_H__
#define SPHERE_H__

#include "object.h"
#include "vector3d.h"

class Sphere : public Object {
public:
  Vector3D center; // center of the sphere
  float r;
  float r2;

  Sphere(Vector3D &c, float radius, int material) {

    center = c;
    r = radius;
    r2 = r * r;
    material_id = material;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &normal, bool isShadowRay) {

    Vector3D L = origin - center;

    float b = 2 * direction.dotProduct(L);
    float c = L.dotProduct(L) - r2;
    // std::cout << "ORG: " << origin.x << " " << origin.y << " " << origin.z <<
    // std::endl;  std::cout << "EHEH: " << 1 << " " << b << " " << c <<
    // std::endl;
    float t0, t1; // these will become roots
    if (!quadraticSolve(1, b, c, t0, t1)) {
      return false;
    }

    if (t0 > t1)
      std::swap(t0, t1);
    // std::cout << "ilginc: " << t0 << " " << t1 << std::endl;

    if (t1 < 0)
      return false; // both roots are negative, no intersection

    if (t0 < 0) {
      t = t1; // if t0 is negative, t1 should be the visible param.
    } else {
      t = t0;
    }

    // TODO HITPOINT RECALCULATED. POSSIBLE OPTIMIZE
    normal = (new Vector3D(L + direction * t))->normalize();

    // std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;

    return true;
  }

  Vector3D &getNormalAt(const Vector3D &hitPoint) {
    Vector3D *normal = new Vector3D(hitPoint - center);
    return normal->normalize();
  }
};

#endif
