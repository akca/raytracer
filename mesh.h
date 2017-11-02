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

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &normal, bool isShadowRay) {

    float tmin = FLOAT_MAX;
    float t2 = FLOAT_MAX;

    for (auto &f : faces) {
      if (f.intersects(origin, direction, t2, isShadowRay) && t2 < tmin) {
        tmin = t2;
        normal = f.normal;
        //std::cout << normal.x << " " << normal.y << " " << normal.z << " " << std::endl;
      }
    }

    t = tmin;

    if (tmin < FLOAT_MAX)
      return true;
    else
      return false;
  }
};

#endif
