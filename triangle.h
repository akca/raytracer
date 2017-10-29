#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "face.h"
#include "object.h"
#include "vector3d.h"

class Triangle : public Object {
public:
  Face face;

  Triangle(Face &f, int m) {
    face = f;
    material_id = m;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t) {
    return face.intersects(origin, direction, t);
  }

  Vector3D &getNormalAt(const Vector3D &hitPoint) { return face.normal; }
};

#endif
