#include "jpeg.h"
#include "texture.h"
#include <iostream>

Texture::Texture(std::string &name, std::string &ip, std::string &dm,
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

void Texture::loadImage() {
  if (image == NULL) {
    int w, h;
    read_jpeg_header(imageName.c_str(), w, h);
    image = new unsigned char[w * h * 3];
    read_jpeg(imageName.c_str(), image, w, h);
  }
}
