#ifndef MESH_H__
#define MESH_H__
#include "face.h"
#include "object.h"
#include "vector3d.h"
#include <limits>
#include <vector>

const float FLOAT_MAX = std::numeric_limits<float>::max();

class Mesh : public Object {
public:
  std::vector<Face> faces;

  Mesh(std::vector<Face> &f, int m) {
    faces = f;
    material_id = m;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &t) const {

    float tmin = FLOAT_MAX;

    for (auto &f : faces) {
      if (f.intersects(origin, direction, t) && t < tmin) {
        tmin = t;
      }
    }

    t = tmin;

    if (tmin < FLOAT_MAX)
      return true;
    else
      return false;
  }

  Vector3D &getNormalAt(const Vector3D &hitPoint) const {}
};

#endif
