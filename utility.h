#ifndef __UTILITY_H__
#define __UTILITY_H__

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1);
float det33(float a, float b, float c, float d, float e, float f, float g,
            float h, float i);

struct Vec3i {
  int x, y, z;
};

struct Vec2f {
  float x, y;
};

struct Vec3f {
  float x, y, z;
};

struct Vec4f {
  float x, y, z, w;
};

#endif
