#ifndef FACE_H__
#define FACE_H__

#include "object.h"
#include "vector3d.h"

class Face {
public:
  Vector3D v1;
  Vector3D v2;
  Vector3D v3;
  Vector3D normal;
  Face() {}

  Face(Vector3D &vt1, Vector3D &vt2, Vector3D &vt3) {
    v1 = vt1;
    v2 = vt2;
    v3 = vt3;
    normal = ((v3 - v2) * (v1 - v2)).normalize();
  }

  bool intersects(const Vector3D &origin, const Vector3D &direction, float &t,
                  bool isShadowRay) {
    /*		std::cout << v1.x <<v1.y<<v1.z<<std::endl;*/
    /*		std::cout << v2.x <<v2.y<<v2.z<<std::endl;*/
    /*		std::cout << v3.x <<v3.y<<v3.z<<std::endl;*/

    // backface culling - ignore back-facing triangles
    if (!isShadowRay && normal.dotProduct(direction) > -0.00001) {
      return false;
    }

    float detA =
        det33(v1.x - v2.x, v1.x - v3.x, direction.x, v1.y - v2.y, v1.y - v3.y,
              direction.y, v1.z - v2.z, v1.z - v3.z, direction.z);

        if (std::abs(detA) < 0.00000001) { // TODO test this value!
          return false;
        }

    // std::cout << detA << std::endl;

    float beta = det33(v1.x - origin.x, v1.x - v3.x, direction.x,
                       v1.y - origin.y, v1.y - v3.y, direction.y,
                       v1.z - origin.z, v1.z - v3.z, direction.z) /
                 detA;

    float gama = det33(v1.x - v2.x, v1.x - origin.x, direction.x,
                       v1.y - v2.y, v1.y - origin.y, direction.y,
                       v1.z - v2.z, v1.z - origin.z, direction.z) /
                 detA;

    float t2 = det33(v1.x - v2.x, v1.x - v3.x, v1.x - origin.x, v1.y - v2.y,
              v1.y - v3.y, v1.y - origin.y, v1.z - v2.z, v1.z - v3.z,
              v1.z - origin.z) /
        detA;

    // TODO FIXME Tmin Tmax should be checked!

    if (t2 > -0.00001 && beta + gama <= 1.00001 && -0.00001 <= beta &&
        -0.00001 <= gama) {
      t = t2;
      return true;
    } else {
      return false;
    }
  }
};

#endif
