#ifndef VECTOR3D_H__
#define VECTOR3D_H__

#include <cmath>
#include "utility.h"

class Vector3D {
public:
    float e[3];

    inline float x() const { return e[0]; }

    inline float y() const { return e[1]; }

    inline float z() const { return e[2]; }

    inline float operator[](int i) const { return e[i]; }

    inline float &operator[](int i) { return e[i]; };

    float getLength() const { return sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }

    Vector3D() {
        e[0] = 0;
        e[1] = 0;
        e[2] = 0;
    }

    Vector3D(const Vector3D &v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
    }

    Vector3D(const Vec3i &v) {
        e[0] = v.x;
        e[1] = v.y;
        e[2] = v.z;
    }

    Vector3D(float i, float j, float k) {
        e[0] = i;
        e[1] = j;
        e[2] = k;
    }

    Vector3D &normalize() {

        float length = getLength();

        e[0] /= length;
        e[1] /= length;
        e[2] /= length;

        return *this;
    }

    Vector3D inverse() const { return Vector3D(-e[0], -e[1], -e[2]); }

    Vector3D operator-(const Vector3D &v) const {
        return Vector3D(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]);
    }

    Vector3D operator+(const Vector3D &v) const {
        return Vector3D(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);
    }

    // Divide by constant
    Vector3D operator/(float cons) const {
        return Vector3D(e[0] / cons, e[1] / cons, e[2] / cons);
    }

    // Multiply by constant
    Vector3D operator*(float cons) const {
        return Vector3D(e[0] * cons, e[1] * cons, e[2] * cons);
    }

    Vector3D power(float cons) const {
        return Vector3D(powf(e[0], cons), powf(e[0], cons), powf(e[2], cons));
    }

    // Vector cross product
    Vector3D operator*(const Vector3D &v) const {
        return Vector3D(e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2], e[0] * v.e[1] - e[1] * v.e[0]);
    }

    float dotProduct(const Vector3D &v) const {
        return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
    }

    Vector3D multiply(const Vector3D &v) const {
        return Vector3D(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]);
    }

    float distance(const Vector3D &v) const {
        return sqrtf(powf((e[0] - v.e[0]), 2) + powf((e[1] - v.e[1]), 2) + powf((e[2] - v.e[2]), 2));
    }

    void applyTransform(const float *v, bool isVector) {

        float newX = v[0] * e[0] + v[1] * e[1] + v[2] * e[2];
        float newY = v[4] * e[0] + v[5] * e[1] + v[6] * e[2];
        float newZ = v[8] * e[0] + v[9] * e[1] + v[10] * e[2];

        if (!isVector) {
            newX += v[3];
            newY += v[7];
            newZ += v[11];
        }
        /*
          std::cout << e[0] << " " << e[1] << " " << e[2] << std::endl;
          std::cout << newX << " " << newY << " " << newZ << std::endl;
          std::cout << std::endl;
        */
        e[0] = newX;
        e[1] = newY;
        e[2] = newZ;
    }

};

#endif
