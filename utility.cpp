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
