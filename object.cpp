#include "object.h"
#include "utility.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void Object::zeroFill(float matrix[]) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      matrix[x + 4 * y] = 0;
    }
  }
}

void Object::createTransformMatrix(std::vector<Vec3f> &t_translation,
                                   std::vector<Vec4f> &t_rotation,
                                   std::vector<Vec3f> &t_scaling,
                                   bool fillZero) {

  if (transformations == "")
    return;

  if (fillZero) {
    transformMatrix = new float[16];
    zeroFill(transformMatrix);
  }

  int tid;
  char ttype;
  int i = 0;

  float newMatrix[16];

  std::string temp;
  std::istringstream stream(transformations);

  while (stream >> temp) {
    zeroFill(&newMatrix[0]);
    std::istringstream stream2(temp);

    stream2 >> ttype;
    stream2 >> tid;

    std::cout << ttype << " " << tid << " " << std::endl << std::endl;

    tid--; // get 0-based index

    // translation
    if (ttype == 't') {

      newMatrix[0] = 1;
      newMatrix[1 * 4 + 1] = 1;
      newMatrix[2 * 4 + 2] = 1;
      newMatrix[3 * 4 + 3] = 1;
      newMatrix[0 * 4 + 3] = t_translation[tid].x;
      newMatrix[1 * 4 + 3] = t_translation[tid].y;
      newMatrix[2 * 4 + 3] = t_translation[tid].z;

      // rotation
    } else if (ttype == 'r') {

      // find min component of u
      Vector3D u(t_rotation[tid].y, t_rotation[tid].z, t_rotation[tid].w);
      float mincoord = std::min({u.x, u.y, u.z});
      Vector3D v;

      if (fabs(u.x - mincoord) < 1e-5) {
        v = Vector3D(0, -u.z, u.y);
      } else if (fabs(u.y - mincoord) < 1e-5) {
        v = Vector3D(-u.z, 0, u.x);
      } else if (fabs(u.z - mincoord) < 1e-5) {
        v = Vector3D(-u.y, u.x, 0);
      } else {
        std::cout << "\n\n\nHHHHHHHHHHHHHHHHHH\\n\n\n\n\n\n\n";
        return;
      }
      Vector3D w = u * v;
      float angle = t_rotation[tid].x * M_PI / 180.0f;

      float M[16]{u.x, u.y, u.z, 0, v.x, v.y, v.z, 0,
                  w.x, w.y, w.z, 0, 0,   0,   0,   1};

      float Minv[16]{u.x, v.x, w.x, 0, u.y, v.y, w.y, 0,
                     u.z, v.z, w.z, 0, 0,   0,   0,   1};

      float cosa = cos(angle);
      float sina = sin(angle);

      float Rx[16]{1, 0, 0, 0, 0, cosa, -sina, 0, 0, sina, cosa, 0, 0, 0, 0, 1};

      mmul44(Minv, Rx, newMatrix);
      mmul44(newMatrix, M, newMatrix);

      // scaling
    } else if (ttype == 's') {

      newMatrix[3 * 4 + 3] = 1;
      newMatrix[0] = t_scaling[tid].x;
      newMatrix[1 * 4 + 1] = t_scaling[tid].y;
      newMatrix[2 * 4 + 2] = t_scaling[tid].z;
    }

    if (i == 0 && fillZero) {
      std::copy(newMatrix, newMatrix + 16, transformMatrix);
    } else {
      mmul44(newMatrix, transformMatrix, transformMatrix);
    }
    i++;
  }
  /*
    for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++) {
        std::cout << *(transformMatrix + 4 * x + y) << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";*/
}
