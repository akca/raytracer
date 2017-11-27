#ifndef MESH_H__
#define MESH_H__

#include <vector>
#include "face.h"

class Mesh : public Object {
public:
  std::vector<Face> faces;

  Mesh(std::vector<Face> &f, int m, int t, std::string &tr) : faces(f) {
    material_id = m;
    texture_id = t;
    transformations = tr;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction,
                  float &tmin, Vector3D &normal, bool isShadowRay);

  Vec2f getTexturePoint(Vector3D &);
  void applyTransform();
};

#endif
