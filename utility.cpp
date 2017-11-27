#include "utility.h"
#include <cmath>

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0,
                    float &t1) {
  float delta = b * b - (4 * a * c);
  // std::cout << delta << std::endl;

  if (delta < 0) {
    return false; // no soln
  } else {
    float q = (b > 0) ? (b + sqrtf(delta)) * -0.5 : (b - sqrtf(delta)) * -0.5;
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

void mmul44(float *i, float *j, float *k) {
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
