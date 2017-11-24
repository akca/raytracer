#include "mesh.h"
#include "face.h"
#include "object.h"
#include "vector3d.h"

bool Mesh::intersects(const Vector3D &origin, const Vector3D &direction,
                      float &tmin, Vector3D &normal, bool isShadowRay) {

  float tmin_new = tmin;
  Vector3D normal_new = normal;

  for (auto &f : faces) {
    if (f.intersects(origin, direction, tmin_new, isShadowRay)) {
      normal_new = f.normal;
    }
  }

  if (tmin_new < tmin) {
    tmin = tmin_new;
    normal = normal_new;
    return true;
  } else
    return false;
}

Vec2f Mesh::getTexturePoint(Vector3D &intersectPoint) {
  Vec2f result = {0.0f, 0.0f};

  return result;
}
