#include "vector3d.h"
#include <cmath>

float Vector3D::getLength() const { return sqrtf(x * x + y * y + z * z); }

Vector3D::Vector3D() {
  x = 0;
  y = 0;
  z = 0;
}

Vector3D::Vector3D(const Vector3D &v) {
  x = v.x;
  y = v.y;
  z = v.z;
}

Vector3D::Vector3D(float i, float j, float k) {
  x = i;
  y = j;
  z = k;
}

Vector3D &Vector3D::normalize() {

  float length = getLength();

  x /= length;
  y /= length;
  z /= length;

  return *this;
}

Vector3D Vector3D::inverse() const { return Vector3D(-x, -y, -z); }

Vector3D Vector3D::operator-(const Vector3D &v) const {
  return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator+(const Vector3D &v) const {
  return Vector3D(x + v.x, y + v.y, z + v.z);
}
// Divide by constant
Vector3D Vector3D::operator/(float cons) const {
  return Vector3D(x / cons, y / cons, z / cons);
}

// Multiply by constant
Vector3D Vector3D::operator*(float cons) const {
  return Vector3D(x * cons, y * cons, z * cons);
}

// Vector cross product
Vector3D Vector3D::operator*(const Vector3D &v) const {
  return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

float Vector3D::dotProduct(const Vector3D &v) const {
  return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D::multiply(const Vector3D &v) const {
  return Vector3D(x * v.x, y * v.y, z * v.z);
}

float Vector3D::distance(const Vector3D &v) const {
  return sqrtf(pow((x - v.x), 2) + pow((y - v.y), 2) + pow((z - v.z), 2));
}

void Vector3D::applyTransform(const float *v, bool isVector) {

  float newX = v[0] * x + v[1] * y + v[2] * z;
  float newY = v[4] * x + v[5] * y + v[6] * z;
  float newZ = v[8] * x + v[9] * y + v[10] * z;

  if (!isVector) {
    newX += v[3];
    newY += v[7];
    newZ += v[11];
  }
  /*
    std::cout << x << " " << y << " " << z << std::endl;
    std::cout << newX << " " << newY << " " << newZ << std::endl;
    std::cout << std::endl;
  */
  x = newX;
  y = newY;
  z = newZ;
}
