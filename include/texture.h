#ifndef TEXTURE_H__
#define TEXTURE_H__

#include "jpeg.h"
#include "utility.h"
#include "vector3d.h"
#include <string>

enum Interpolation { NEAREST, BILINEAR };
enum DecalMode { REPLACE_KD, BLEND_KD, REPLACE_ALL };
enum Appereance { REPEAT, CLAMP };

class Texture {
private:
  std::string imageName;
  Interpolation interpolation;
  DecalMode decalMode;
  Appereance appereance;
  unsigned char *image = NULL;
  int width;
  int height;

public:
  Texture(std::string &name, std::string &ip, std::string &dm,
          std::string &ap) {
    imageName = name;

    if (ip == "nearest") {
      interpolation = NEAREST;
    } else if (ip == "bilinear") {
      interpolation = BILINEAR;
    }
    if (dm == "replace_kd") {
      decalMode = REPLACE_KD;
    } else if (dm == "blend_kd") {
      decalMode = BLEND_KD;
    } else if (dm == "replace_all") {
      decalMode = REPLACE_ALL;
    }
    if (ap == "repeat") {
      appereance = REPEAT;
    } else if (ap == "clamp") {
      appereance = CLAMP;
    }
  }

  void loadImage() {
    if (image == NULL) {
      read_jpeg_header(imageName.c_str(), width, height);
      image = new unsigned char[width * height * 3];
      read_jpeg(imageName.c_str(), image, width, height);
    }
  }

  int getWidth() { return width; }
  int getHeight() { return height; }
  Interpolation getInterpolation() { return interpolation; }
  DecalMode getDecalMode() { return decalMode; }
  Appereance getAppereance() { return appereance; }

  // TODO move it to main! and OPTIMIZE multiplication
  Vector3D getColor(Vec2f &texturePoint, Interpolation interpolation) {
    float x = texturePoint.x;
    float y = texturePoint.y;

    switch (appereance) {
    case CLAMP:
      x = std::min(1.0f, x);
      y = std::min(1.0f, y);
      x = std::max(0.0f, x);
      y = std::max(0.0f, y);
      break;
    case REPEAT:
      if (x > 1.0f || x < 0.0f)
        x = x - floor(x);
      if (y > 1.0f || y < 0.0f)
        y = y - floor(y);
      break;
    }
    x *= width;
    y *= height;

    int p = floor(x);
    int q = floor(y);

    int index = (p + q * width) * 3;
    int index2 = index + 3 * width;
    Vector3D color;

    switch (interpolation) {
    case NEAREST:
      color = Vector3D(image[index], image[index + 1], image[index + 2]);
      break;
    case BILINEAR:
      float dx = x - p;
      float dy = y - q;
      color =
          Vector3D(image[index], image[index + 1], image[index + 2]) *
              ((1 - dx) * (1 - dy)) +
          Vector3D(image[index + 3], image[index + 4], image[index + 5]) *
              (dx * (1 - dy)) +
          Vector3D(image[index2], image[index2 + 1], image[index2 + 2]) *
              ((1 - dx) * dy) +
          Vector3D(image[index2 + 3], image[index2 + 4], image[index2 + 5]) *
              (dx * dy);
      break;
    }
    return color;
  }
};

#endif
