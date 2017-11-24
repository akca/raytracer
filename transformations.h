#ifndef TRANSFORMATIONS_H__
#define TRANSFORMATIONS_H__

#include "parser.h"
#include "vector3d.h"
#include <vector>

class Transformations {
public:
  std::vector<parser::Vec3f> translation;
  std::vector<parser::Vec4f> rotation;
  std::vector<parser::Vec3f> scaling;

  ~Transformations() {}

  Transformations() {}
};

#endif
