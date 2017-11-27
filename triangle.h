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

  Triangle() {}

  Triangle(Vector3D &vt1, Vector3D vt2, Vector3D vt3, int m, int t)
      : v1(vt1), edge1(vt2), edge2(vt3), normal((vt2 * vt3).normalize()) {
    material_id = m;
    texture_id = t;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &normal, bool isShadowRay);

  Vec2f getTexturePoint(Vector3D &);
  void applyTransform();
};

#endif
