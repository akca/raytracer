#include "parser.h"
#include "ppm.h"
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

const float FLOAT_MAX = std::numeric_limits<float>::max();

Vector3D shade(parser::Scene &scene, Vector3D &cameraPosition,
               Vector3D &direction, int recursionDepth) {

  Object *intersectObject = NULL;
  float tmin = FLOAT_MAX;
  Vector3D normal;

  for (auto &object : scene.objects) {

    if (object->intersects(cameraPosition, direction, tmin, normal, false)) {
      // tmin = t;
      // std::cout << tmin << std::endl;
      intersectObject = object;
    }
  }
  Vector3D pixelColor;

  // if intersection with an object found
  if (intersectObject != NULL) {

    Vector3D intersectPoint = cameraPosition + direction * tmin;
    // std::cout << intersectPoint.x << " " <<intersectPoint.y << " " <<
    // intersectPoint.z << " " << std::endl;

    Vector3D &kDiffuse = scene.materials[intersectObject->material_id].diffuse;
    Vector3D &kAmbient = scene.materials[intersectObject->material_id].ambient;
    Vector3D &kSpecular =
        scene.materials[intersectObject->material_id].specular;
    Vector3D &kMirror = scene.materials[intersectObject->material_id].mirror;

    // ambient light
    pixelColor = kAmbient.multiply(scene.ambient_light);

    for (parser::PointLight &light : scene.point_lights) {

      Vector3D wi = (light.position - intersectPoint).normalize();

      // std::cout << wi.x << " " <<wi.y << " " << wi.z << " " << std::endl;

      Vector3D shadowRayOrigin = intersectPoint + wi * scene.shadow_ray_epsilon;
      Vector3D tmp_vec;
      float stmin = FLOAT_MAX;
      float lightDistance = intersectPoint.distance(light.position);
      bool underShadow = false;

      // shadow ray
      for (auto &sobject : scene.objects) {
        if (sobject->intersects(shadowRayOrigin, wi, stmin, tmp_vec, true)) {
          if (lightDistance > stmin) {
            underShadow = true;
            break;
          }
        }
      }

      if (!underShadow) {
        // std::cout << shadowRayOrigin.distance(light.position) << " "
        //           << stmin << std::endl;

        float costheta_diff = std::max(float(0), wi.dotProduct(normal));
        // std::cout << costheta_diff << std::endl;
        // std::cout << normal.x << " " << normal.y << " " << normal.z << " " <<
        // std::endl;

        float distance2 = pow(lightDistance, 2);

        // diffuse shading
        pixelColor =
            pixelColor +
            (kDiffuse * costheta_diff).multiply(light.intensity / distance2);

        Vector3D halfVector =
            (wi + direction.inverse()).normalize(); // for specular

        float costheta_spec = std::max(float(0), normal.dotProduct(halfVector));

        // specular shading
        pixelColor =
            pixelColor +
            (kSpecular *
             pow(costheta_spec,
                 scene.materials[intersectObject->material_id].phong_exponent))
                .multiply(light.intensity / distance2);
      }
    }

    // reflection
    if ((kMirror.x > 0.0001 || kMirror.y > 0.0001 || kMirror.z > 0.0001) &&
        recursionDepth < scene.max_recursion_depth) {

      Vector3D wr =
          (direction + (normal * 2) * (normal.dotProduct(direction.inverse())))
              .normalize(); // for reflectance

      Vector3D shadowRayOrigin = intersectPoint + wr * scene.shadow_ray_epsilon;

      pixelColor = pixelColor + kMirror.multiply(shade(scene, shadowRayOrigin,
                                                       wr, recursionDepth + 1));
    }

    // TODO FIX ROUNDING ###############################

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

  // TODO: OPTIMIZE: MOVE THESE AWAY FROM THIS FUNCTION.
  // TODO: THESE ARE COMMON TO ALL THREADS.
  Vector3D cameraPosition(camera->position.x, camera->position.y,
                          camera->position.z);
  Vector3D cameraGaze(camera->gaze.x, camera->gaze.y, camera->gaze.z);
  cameraGaze.normalize();
  Vector3D cameraUp(camera->up.x, camera->up.y, camera->up.z);
  Vector3D cameraRight =
      (cameraGaze * cameraUp).normalize(); // cross product TODO
  cameraUp = cameraRight * cameraGaze;
  // std::cout << "cameraRight: " << cameraRight.x << " " << cameraRight.y
  // << " " << cameraRight.z << std::endl;

  Vector3D centerOfPlane = cameraPosition + cameraGaze * camera->near_distance;
  Vector3D planeStartPoint = centerOfPlane +
                             (cameraRight * camera->near_plane.x) +
                             (cameraUp * camera->near_plane.w);

  int i = startHeight * imageWidth * 3;

  for (int y = startHeight; y < endHeight; ++y) {
    for (int x = 0; x < imageWidth; ++x) {
      // std::cout << x << " " << y << std::endl;

      float pixelPositionX = (camera->near_plane.y - camera->near_plane.x) *
                             (x + 0.5) / imageWidth; // su
      float pixelPositionY = (camera->near_plane.w - camera->near_plane.z) *
                             (y + 0.5) / imageHeight; // sv

      Vector3D pixelPosition = planeStartPoint +
                               (cameraRight * pixelPositionX) -
                               (cameraUp * pixelPositionY);

      Vector3D direction = pixelPosition - cameraPosition;
      // std::cout << "DIR: " << direction.x << " " << direction.y << " " <<
      // direction.z << std::endl;
      direction.normalize();

      Vector3D pixelColor = shade(*scene, cameraPosition, direction, 0);

      // std::cout << "i: " << i/3 << "- " << pixelColor.x <<" " << pixelColor.y
      // <<" " << pixelColor.z << std::endl;
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
  }
}
