#ifndef OBJECT_H__
#define OBJECT_H__

#include "utility.h"
#include "vector3d.h"
#include <string>
#include <vector>

class Object {
public:
  virtual ~Object() {}

  virtual bool intersects(const Vector3D &, const Vector3D &, float &,
                          Vector3D &, bool isShadowRay) = 0;
  virtual Vec2f getTexturePoint(Vector3D &) = 0;

  void createTransformMatrix(std::vector<Vec3f> &t_translation,
                             std::vector<Vec4f> &t_rotation,
                             std::vector<Vec3f> &t_scaling, bool fillZero);
  virtual void applyTransform() = 0;

  // TODO OPTIMIZE
  std::string transformations;
  float *transformMatrix = NULL;

  int material_id;
  int texture_id;

private:
  void zeroFill(float matrix[]);
};

#endif
