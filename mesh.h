#ifndef MESH_H__
#define MESH_H__
#include "face.h"
#include "object.h"
#include "vector3d.h"
#include <iostream>
#include <limits>
#include <vector>

class Mesh : public Object {
public:
  std::vector<Face> faces;

  Mesh(std::vector<Face> &f, int m, int t) : faces(f) {
    material_id = m;
    texture_id = t;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &tmin, Vector3D &normal, bool isShadowRay);
};

#endif
