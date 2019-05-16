#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <vector>

struct Vec3i {
    int x, y, z;
};

struct Vec4i {
    int x, y, z, w;
};

struct Vec2f {
    float x = 0, y = 0;

    Vec2f() = default;

    Vec2f(float x, float y) : x(x), y(y) {}
};

struct Vec3f {
    float x, y, z;
};

struct Vec4f {
    float x, y, z, w;
};

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1);

float det33(float a, float b, float c, float d, float e, float f, float g,
            float h, float i);

float det44(float i[4][4], float j[4][4], float k[4][4]);

void mmul44(const float *i, const float *j, float *k);

void zeroFill(float matrix[]);

float *createTransformMatrix(std::vector<Vec3f> &t_translation,
                             std::vector<Vec4f> &t_rotation,
                             std::vector<Vec3f> &t_scaling, bool inverse,
                             const std::string &transformations);

float maxScaleMultiplier(std::vector<Vec3f> &t_scaling,
                         const std::string &transformations);

#endif
