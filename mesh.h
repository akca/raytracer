#ifndef MESH_H__
#define MESH_H__
#include "face.h"
#include "object.h"
#include "vector3d.h"
#include <limits>
#include <vector>
#include <iostream>

const float FLOAT_MAX = std::numeric_limits<float>::max();

class Mesh : public Object {
public:
  std::vector<Face> faces;

  Mesh(std::vector<Face> &f, int m) {
    faces = f;
    material_id = m;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &tmin,
                  Vector3D &normal, bool isShadowRay) {

    float tmin_new = tmin;

    for (auto &f : faces) {
      if (f.intersects(origin, direction, tmin_new, isShadowRay)) {
        normal = f.normal;
        //std::cout << normal.x << " " << normal.y << " " << normal.z << " " << std::endl;
      }
    }


    if (tmin_new > 0 && tmin_new < tmin) {
      tmin = tmin_new;
      return true;
    }
    else
      return false;
  }
};

#endif
