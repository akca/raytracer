#ifndef OBJECT_H__
#define OBJECT_H__

#include "vector3d.h"

class Object {
public:
  virtual ~Object() {}

  virtual bool intersects(const Vector3D &, const Vector3D &, float &,
                          Vector3D &, bool isShadowRay) = 0;

  Vector3D color;
  int material_id;
};

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1);
float det33(float a, float b, float c, float d, float e, float f, float g,
            float h, float i);

#endif
