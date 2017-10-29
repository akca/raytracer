#ifndef VECTOR3D_H__
#define VECTOR3D_H__

#include <cmath>
#include <iostream>

class Vector3D {
public:
  float x;
  float y;
  float z;

  float getLength() const { return sqrt(x * x + y * y + z * z); }

  Vector3D &normalize() {

    float length = getLength();

    x /= length;
    y /= length;
    z /= length;

    return *this;
  }

  Vector3D() {
    x = 0;
    y = 0;
    z = 0;
  }

  Vector3D(const Vector3D &v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  Vector3D(float i, float j, float k) {
    x = i;
    y = j;
    z = k;
  }

  Vector3D inverse() const { return Vector3D(-x, -y, -z); }

  Vector3D operator-(const Vector3D &v) const {
    return Vector3D(x - v.x, y - v.y, z - v.z);
  }

  Vector3D operator+(const Vector3D &v) const {
    return Vector3D(x + v.x, y + v.y, z + v.z);
  }
  // Divide by constant
  Vector3D operator/(float cons) const {
    return Vector3D(x / cons, y / cons, z / cons);
  }

  // Multiply by constant
  Vector3D operator*(float cons) const {
    return Vector3D(x * cons, y * cons, z * cons);
  }

  // Vector cross product TODO
  Vector3D operator*(const Vector3D &v) const {
    return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  float dotProduct(const Vector3D &v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  Vector3D multiply(const Vector3D &v) const {
    return Vector3D(x * v.x, y * v.y, z * v.z);
  }

  float distance(const Vector3D &v) const {
    return sqrt(pow((x - v.x), 2) + pow((y - v.y), 2) + pow((z - v.z), 2));
  }
};

#endif
