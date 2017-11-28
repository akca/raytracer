#ifndef VECTOR3D_H__
#define VECTOR3D_H__

#include <cmath>

class Vector3D {
public:
  float x;
  float y;
  float z;

  Vector3D();
  Vector3D(const Vector3D &v);
  Vector3D(float i, float j, float k);
  float getLength() const;
  Vector3D &normalize();
  Vector3D inverse() const;
  Vector3D operator-(const Vector3D &v) const;
  Vector3D operator+(const Vector3D &v) const;
  float dotProduct(const Vector3D &v) const;
  Vector3D multiply(const Vector3D &v) const;
  void applyTransform(const float *v, bool isVector);
  float distance(const Vector3D &v) const;

  // Divide by constant
  Vector3D operator/(float cons) const;

  // Multiply by constant
  Vector3D operator*(float cons) const;

  // Vector cross product
  Vector3D operator*(const Vector3D &v) const;
};

#endif
