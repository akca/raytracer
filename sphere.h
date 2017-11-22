#ifndef SPHERE_H__
#define SPHERE_H__

#include "object.h"
#include "vector3d.h"

class Sphere : public Object {
public:
  Vector3D center; // center of the sphere
  float r;
  float r2;

  Sphere(Vector3D &c, float radius, int m, int t)
      : center(c), r(radius), r2(r * r) {
    material_id = m;
    texture_id = t;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &normal, bool isShadowRay);
};

#endif
