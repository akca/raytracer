#ifndef SPHERE_H__
#define SPHERE_H__

#include "object.h"
#include "vector3d.h"
#include <algorithm>

class Sphere : public Object {
public:
  Vector3D center; // center of the sphere
  float r;
  float r2;
  float *transformMatrix = NULL;
  float *invTransformMatrix = NULL;
  float invTransposeTransformMatrix[16];

  Sphere(Vector3D &c, float radius, int m, int t)
      : center(c), r(radius), r2(r * r) {
    material_id = m;
    texture_id = t;
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  Vector3D &intersectPoint, Vector3D &normal, bool isShadowRay,
                  Vec2f &texCoord) {

    Vector3D newOrigin = origin;
    Vector3D newDirection = direction;

    if (invTransformMatrix) {
      newDirection.applyTransform(invTransformMatrix, true);
      newDirection.normalize();
      newOrigin.applyTransform(invTransformMatrix, false);
    }
    Vector3D L = newOrigin - center;

    float b = 2 * newDirection.dotProduct(L);
    float c = L.dotProduct(L) - r2;

    float t0, t1; // these will become roots
    if (!quadraticSolve(1, b, c, t0, t1)) {
      return false;
    }

    if (t0 > t1)
      std::swap(t0, t1);

    if (t1 < 0)
      return false; // both roots are negative, no intersection

    float tmin;
    if (t0 < 0) {
      tmin = t1; // if t0 is negative, t1 should be the visible param.
    } else {
      tmin = t0;
    }

    // if a closer object is already there, this object is not visible
    if (tmin > t)
      return false;
    else {
      t = tmin;

      intersectPoint = newOrigin + newDirection * t;

      if (!isShadowRay) {

        normal = (intersectPoint - center).normalize();

        if (texture_id != -1) {
          Vector3D relative = intersectPoint - center;
          texCoord.x = -atan2(relative.z, relative.x) / (2 * M_PI) + 0.5f; // u
          texCoord.y = acos(relative.y / r) / M_PI;                        // v
        }

        if (transformMatrix) {
          intersectPoint.applyTransform(transformMatrix, false);
          normal.applyTransform(invTransposeTransformMatrix, true);
          normal.normalize();
        }
      }
      return true;
    }
  }
};

#endif
