#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "object.h"
#include "vector3d.h"

class Triangle : public Object {
public:
  Vector3D v1;
  Vector3D edge1;
  Vector3D edge2;
  Vector3D normal;
  // texcoords
  Vec2f texCoord1;
  Vec2f texCoord2;
  Vec2f texCoord3;

  Triangle() {}

  Triangle(Vector3D &vt1, Vector3D vt2, Vector3D vt3, int material, int texture,
           Vec2f &texCoord1, Vec2f &texCoord2, Vec2f &texCoord3)
      : v1(vt1), edge1(vt2), edge2(vt3), normal((vt2 * vt3).normalize()),
        texCoord1(texCoord1), texCoord2(texCoord2), texCoord3(texCoord3) {
    material_id = material;
    texture_id = texture;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &intersectPoint, Vector3D &normal, bool isShadowRay,
                  Vec2f &texCoordData);
};

#endif
