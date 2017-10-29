#ifndef OBJECT_H__
#define OBJECT_H__

#include "vector3d.h"

class Object {
public:
  virtual ~Object() {}

  virtual bool intersects(const Vector3D &, const Vector3D &,
                          float &) = 0;
  virtual Vector3D &getNormalAt(const Vector3D &hitPoint) = 0;

  Vector3D color; // TODO change data type!
  int material_id;
};

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1);
float det33(float a, float b, float c, float d, float e, float f, float g,
            float h, float i);
/*
struct Vec3f
{
float x, y, z;
};
*/
#endif
