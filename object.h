#ifndef OBJECT_H__
#define OBJECT_H__

#include "utility.h"
#include "vector3d.h"
#include <string>
#include <vector>

class Object {
public:
  virtual ~Object() {}

  virtual bool intersects(const Vector3D &, const Vector3D &, float &,
                          Vector3D &intersectPoint, Vector3D &,
                          bool isShadowRay, Vec2f &texCoordData) = 0;

  int material_id;
  int texture_id;
};

#endif
