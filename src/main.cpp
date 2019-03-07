#include "../include/parser.h"
#include "../include/ppm.h"
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

Vector3D shade(parser::Scene &scene, Vector3D &rayOrigin, Vector3D &direction,
               int recursionDepth) {

  Object *intersectObject = NULL;
  float tmin = std::numeric_limits<float>::max();
  Vector3D normal, pixelColor;
  Vector3D intersectPoint;
  Vec2f texCoord;

  for (auto &object : scene.objects) {
    if (object->intersects(rayOrigin, direction, tmin, intersectPoint, normal,
                           false, texCoord)) {
      intersectObject = object;
    }
  }

  // if intersection with an object found
  if (intersectObject != NULL) {

    intersectPoint = rayOrigin + direction * tmin;

    Vector3D &kDiffuse = scene.materials[intersectObject->material_id].diffuse;
    Vector3D &kAmbient = scene.materials[intersectObject->material_id].ambient;
    Vector3D &kSpecular =
        scene.materials[intersectObject->material_id].specular;
    Vector3D &kMirror = scene.materials[intersectObject->material_id].mirror;

    // ambient light
    pixelColor = kAmbient.multiply(scene.ambient_light);

    for (auto &light : scene.point_lights) {

      Vector3D wi = light.position - intersectPoint;
      float lightDistance = intersectPoint.distance(light.position);
      wi = wi / lightDistance; // normalize

      Vector3D shadowRayOrigin = intersectPoint + wi * scene.shadow_ray_epsilon;

      // search objects between light and intersection point
      float stmin = lightDistance;
      bool underShadow = false;
      Vector3D tmp_vec;
      Vec2f tmp_vec2;

      // shadow ray
      for (auto &sobject : scene.objects) {
        if (sobject->intersects(shadowRayOrigin, wi, stmin, tmp_vec, tmp_vec,
                                true, tmp_vec2)) {
          underShadow = true;
          break;
        }
      }

      if (!underShadow) {
        bool isReplaceAll = false;

        float costheta_diff = std::max(float(0), wi.dotProduct(normal));
        float distance2 = pow(lightDistance, 2);

        // diffuse shading
        if (intersectObject->texture_id == -1) {
          pixelColor =
              pixelColor +
              (kDiffuse * costheta_diff).multiply(light.intensity / distance2);
        } else {
          // if the object have texture
          Texture &texture = scene.textures[intersectObject->texture_id];

          DecalMode decalMode = texture.getDecalMode();

          Vector3D textureColor =
              texture.getColor(texCoord, texture.getInterpolation());

          switch (decalMode) {
          case REPLACE_KD:
            pixelColor =
                pixelColor + (textureColor / 256 * costheta_diff)
                                 .multiply(light.intensity / distance2);
            break;
          case BLEND_KD:
            pixelColor =
                pixelColor +
                ((kDiffuse + textureColor / 256) * 0.5f * costheta_diff)
                    .multiply(light.intensity / distance2);

            break;
          case REPLACE_ALL:
            pixelColor = textureColor;
            isReplaceAll = true;
            break;
          }
        }

        if (!isReplaceAll) {

          Vector3D halfVector =
              (wi + direction.inverse()).normalize(); // for specular

          float costheta_spec =
              std::max(float(0), normal.dotProduct(halfVector));

          // specular shading
          pixelColor =
              pixelColor +
              (kSpecular *
               pow(costheta_spec, scene.materials[intersectObject->material_id]
                                      .phong_exponent))
                  .multiply(light.intensity / distance2);
        }
      }
    }

    // reflection
    if ((kMirror.x + kMirror.y + kMirror.z > 0.0f) &&
        recursionDepth < scene.max_recursion_depth) {

      Vector3D wr =
          (direction + (normal * 2) * (normal.dotProduct(direction.inverse())))
              .normalize(); // for reflectance

      Vector3D rayOriginEps = intersectPoint + wr * scene.shadow_ray_epsilon;

      pixelColor = pixelColor + kMirror.multiply(shade(scene, rayOriginEps, wr,
                                                       recursionDepth + 1));
    }

    pixelColor.x = std::min((int)round(pixelColor.x), 255); // r
    pixelColor.y = std::min((int)round(pixelColor.y), 255); // g
    pixelColor.z = std::min((int)round(pixelColor.z), 255); // b
  } else {
    pixelColor.x = scene.background_color.x; // r
    pixelColor.y = scene.background_color.y; // g
    pixelColor.z = scene.background_color.z; // b
  }
  return pixelColor;
}

void trace(parser::Scene *scene, parser::Camera *camera, int startHeight,
           int endHeight, int imageWidth, int imageHeight,
           unsigned char *image) {

  int i = startHeight * imageWidth * 3;

  float nearPlaneWidth = camera->near_plane.y - camera->near_plane.x;
  float nearPlaneHeight = camera->near_plane.w - camera->near_plane.z;

  for (int y = startHeight; y < endHeight; ++y) {
    for (int x = 0; x < imageWidth; ++x) {

      float pixelPositionX = nearPlaneWidth * (x + 0.5) / imageWidth;   // su
      float pixelPositionY = nearPlaneHeight * (y + 0.5) / imageHeight; // sv

      Vector3D pixelPosition = camera->planeStartPoint +
                               (camera->right * pixelPositionX) -
                               (camera->up * pixelPositionY);

      Vector3D direction = pixelPosition - camera->position;
      direction.normalize();

      Vector3D pixelColor = shade(*scene, camera->position, direction, 0);

      image[i++] = pixelColor.x; // r
      image[i++] = pixelColor.y; // g
      image[i++] = pixelColor.z; // b
    }
  }
}

int main(int argc, char *argv[]) {
  parser::Scene scene;

  scene.loadFromXml(argv[1]);
  using std::cout;
  using std::endl;
  using std::thread;

  for (parser::Camera camera : scene.cameras) {
    std::cout << "RENDERING STARTED: " << camera.image_name << std::endl;

    int width = camera.image_width;
    int height = camera.image_height;

    unsigned char *image = new unsigned char[width * height * 3];

    size_t threadCount = 128;
    int partition = height / threadCount;
    int startHeight = 0;
    int endHeight = partition;
    std::vector<thread> threads;

    for (size_t i = 0; i < threadCount; i++) {
      threads.emplace_back(trace, &scene, &camera, startHeight, endHeight,
                           width, height, image);
      startHeight = endHeight;
      if (i >= threadCount - 2) {
        endHeight = height;
      } else {
        endHeight += partition;
      }
    }
    for (std::thread &t1 : threads) {
      t1.join();
    }
    threads.clear();

    // trace(&scene, &camera, 0, height, width, height, image);

    write_ppm((camera.image_name).c_str(), image, width, height);
    delete[] image;
  }

  // deallocate objects
  for (auto &obj : scene.objects) {
    delete obj;
  }
}
