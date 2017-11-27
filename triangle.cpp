#include "parser.h"
#include "triangle.h"

bool Triangle::intersects(const Vector3D &origin, const Vector3D &direction,
                          float &t, Vector3D &normal, bool isShadowRay) {
  if (face.intersects(origin, direction, t, isShadowRay)) {
    normal = face.normal;
    return true;
  }
  return false;
}

Vec2f Triangle::getTexturePoint(Vector3D &intersectPoint) {
  Vec2f result = {0.0f, 0.0f};

  return result;
}

void Triangle::applyTransform() {
  if (transformMatrix == NULL) {
    return;
  }
  face.edge1 = face.edge1 + face.v1;
  face.edge2 = face.edge2 + face.v1;
  face.v1.applyTransform(transformMatrix);
  face.edge1.applyTransform(transformMatrix);
  face.edge2.applyTransform(transformMatrix);
  face.edge1 = face.edge1 - face.v1;
  face.edge2 = face.edge2 - face.v1;
  face.normal = (face.edge1 * face.edge2).normalize();

  delete[] transformMatrix;
  transformMatrix = NULL;
}
