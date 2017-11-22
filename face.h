#ifndef FACE_H__
#define FACE_H__

#include "object.h"
#include "vector3d.h"

class Face {
public:
  Vector3D v1;
  Vector3D edge1;
  Vector3D edge2;
  Vector3D normal;
  
  Face() {}

  Face(Vector3D &vt1, Vector3D vt2, Vector3D vt3)
      : v1(vt1), edge1(vt2), edge2(vt3), normal((vt2 * vt3).normalize()) {}

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &tmin, bool isShadowRay);
};

#endif
