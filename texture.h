#ifndef TEXTURE_H__
#define TEXTURE_H__

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
  unsigned char* image = NULL;

public:
  Texture(std::string &name, std::string &ip, std::string &dm, std::string &ap);
  void loadImage();
};

#endif
