#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "face.h"
#include "object.h"
#include "vector3d.h"

class Triangle : public Object {
public:
  Face face;

  Triangle(Face &f, int m, int t) : face(f) {
    material_id = m;
    texture_id = t;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &normal, bool isShadowRay);

  Vector3D &getNormalAt(const Vector3D &hitPoint) { return face.normal; }

  Vec2f getTexturePoint(Vector3D &);
};

#endif
