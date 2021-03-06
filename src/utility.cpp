#include <cmath>

#include <cmath>

#include "utility.h"
#include "vector3d.h"
#include <algorithm>
#include <sstream>

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1) {
    float delta = b * b - (4 * a * c);
    // std::cout << delta << std::endl;

    if (delta < 0) {
        return false; // no soln
    } else {
        float q = (b > 0) ? (b + sqrtf(delta)) * -0.5f : (b - sqrtf(delta)) * -0.5f;
        t0 = q / a;
        t1 = c / q;
    }

    return true;
}

/*
returns determinant of matrix:

a b c
d e f
g h i
*/
float det33(float a, float b, float c, float d, float e, float f, float g,
            float h, float i) {
    return a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
}

void mmul44(const float *i, const float *j, float *k) {
    float result[16];

    for (int x = 0; x < 4; x++) {   // rows
        for (int y = 0; y < 4; y++) { // columns
            result[4 * x + y] = 0;
            for (int z = 0; z < 4; z++) { // four elements are added
                result[4 * x + y] += i[4 * x + z] * j[4 * z + y];
            }
        }
    }

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            k[4 * x + y] = result[4 * x + y];
        }
    }
}

void zeroFill(float matrix[]) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            matrix[x + 4 * y] = 0;
        }
    }
}

float *createTransformMatrix(std::vector<Vec3f> &t_translation,
                             std::vector<Vec4f> &t_rotation,
                             std::vector<Vec3f> &t_scaling, bool inverse,
                             const std::string &transformations) {

    if (transformations.empty())
        return nullptr;

    auto transformMatrix = new float[16];

    zeroFill(transformMatrix);

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

        // std::cout << ttype << " " << tid << " " << std::endl << std::endl;

        tid--; // get 0-based index

        // translation
        if (ttype == 't') {

            newMatrix[0] = 1;
            newMatrix[1 * 4 + 1] = 1;
            newMatrix[2 * 4 + 2] = 1;
            newMatrix[3 * 4 + 3] = 1;
            if (inverse) {
                newMatrix[0 * 4 + 3] = -t_translation[tid].x;
                newMatrix[1 * 4 + 3] = -t_translation[tid].y;
                newMatrix[2 * 4 + 3] = -t_translation[tid].z;
            } else {
                newMatrix[0 * 4 + 3] = t_translation[tid].x;
                newMatrix[1 * 4 + 3] = t_translation[tid].y;
                newMatrix[2 * 4 + 3] = t_translation[tid].z;
            }
            // rotation
        } else if (ttype == 'r') {

            // find min component of u
            Vector3D u(t_rotation[tid].y, t_rotation[tid].z, t_rotation[tid].w);
            float mincoord = std::min({u.x(), u.y(), u.z()});
            Vector3D v;

            if (std::fabs(u.x() - mincoord) < 1e-5) {
                v = Vector3D(0, -u.z(), u.y());
            } else if (std::fabs(u.y() - mincoord) < 1e-5) {
                v = Vector3D(-u.z(), 0, u.x());
            } else if (std::fabs(u.z() - mincoord) < 1e-5) {
                v = Vector3D(-u.y(), u.x(), 0);
            } else {
                return nullptr;
            }
            Vector3D w = u * v;
            float angle;

            angle = t_rotation[tid].x * M_PI / 180.0f;

            float M[16]{u.x(), u.y(), u.z(), 0, v.x(), v.y(), v.z(), 0,
                        w.x(), w.y(), w.z(), 0, 0, 0, 0, 1};

            float Minv[16]{u.x(), v.x(), w.x(), 0, u.y(), v.y(), w.y(), 0,
                           u.z(), v.z(), w.z(), 0, 0, 0, 0, 1};

            float cosa = std::cos(angle);
            float sina = std::sin(angle);
            float Rx[16]{1, 0, 0, 0, 0, cosa, -sina, 0, 0, sina, cosa, 0, 0, 0, 0, 1};

            if (inverse) {
                Rx[6] *= -1;
                Rx[9] *= -1;
            }

            mmul44(Minv, Rx, newMatrix);
            mmul44(newMatrix, M, newMatrix);

            // scaling
        } else if (ttype == 's') {

            newMatrix[3 * 4 + 3] = 1;

            if (inverse) {
                newMatrix[0] = 1.0f / t_scaling[tid].x;
                newMatrix[1 * 4 + 1] = 1.0f / t_scaling[tid].y;
                newMatrix[2 * 4 + 2] = 1.0f / t_scaling[tid].z;
            } else {
                newMatrix[0] = t_scaling[tid].x;
                newMatrix[1 * 4 + 1] = t_scaling[tid].y;
                newMatrix[2 * 4 + 2] = t_scaling[tid].z;
            }
        }

        if (i == 0) {
            std::copy(newMatrix, newMatrix + 16, transformMatrix);
        } else {
            if (inverse) {
                mmul44(transformMatrix, newMatrix, transformMatrix);
            } else {
                mmul44(newMatrix, transformMatrix, transformMatrix);
            }
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
        std::cout << "\n";
    */
    return transformMatrix;
}

/**
 * Calculate max scale multiplier.
 *
 * @param t_scaling all scale transformations
 * @param transformations transformation string
 * @return maximum real cumulative scale multiplier of 3 axis.
 * E.g. if scaled with (1.2, 3.5, 5.0), return 5.0.
 */
float maxScaleMultiplier(std::vector<Vec3f> &t_scaling,
                         const std::string &transformations) {

    if (transformations.empty())
        return 0.0f;

    // cumulative scale coefficients for each axis
    float scaleCoefficient[3]{1.0f, 1.0f, 1.0f};

    int tid;
    char ttype;

    std::string temp;
    std::istringstream stream(transformations);

    while (stream >> temp) {
        std::istringstream stream2(temp);

        stream2 >> ttype;
        stream2 >> tid;

        // scale
        if (ttype == 's') {

            scaleCoefficient[0] *= t_scaling[tid].x;
            scaleCoefficient[1] *= t_scaling[tid].y;
            scaleCoefficient[2] *= t_scaling[tid].z;
        }

    }

    return std::max(scaleCoefficient[0], std::max(scaleCoefficient[1], scaleCoefficient[2]));
}