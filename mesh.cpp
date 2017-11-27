#include "face.h"
#include "mesh.h"
#include "object.h"
#include "vector3d.h"
#include <iostream>

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

void Mesh::applyTransform() {
  if (transformMatrix == NULL) {
    return;
  }
  /*
    float invTrnsMatrix[16];

    invert(transformMatrix, invTrnsMatrix);
    // transpose it
    for (int x = 0; x < 4; x++) {
      for (int y = x + 1; y < 4; y++) {
        std::swap(invTrnsMatrix[4 * x + y], invTrnsMatrix[4 * y + x]);
      }
    }*/

  for (auto &face : faces) {

    // // std::cout << face.normal.x << " " << face.normal.y << " " <<
    // // face.normal.z << std::endl;
    // face.normal.applyTransform(invTrnsMatrix);
    // face.normal.normalize();
    // // std::cout << face.normal.x << " " << face.normal.y << " " <<
    // // face.normal.z << std::endl << std::endl;

    // transform vertices
    face.edge1 = face.edge1 + face.v1;
    face.edge2 = face.edge2 + face.v1;
    face.v1.applyTransform(transformMatrix);
    face.edge1.applyTransform(transformMatrix);
    face.edge2.applyTransform(transformMatrix);
    face.edge1 = face.edge1 - face.v1;
    face.edge2 = face.edge2 - face.v1;
    face.normal = (face.edge1 * face.edge2).normalize();
  }
  delete[] transformMatrix;
  transformMatrix = NULL;
}
