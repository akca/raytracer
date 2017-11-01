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

    float t = FLOAT_MAX;
    if (object->intersects(cameraPosition, direction, t, normal, false) &&
        t < tmin) {
      tmin = t;
      intersectObject = object;
    }
  }
  Vector3D pixelColor;

  // if intersection with an object found
  if (tmin < FLOAT_MAX) {

    Vector3D intersectPoint = cameraPosition + direction * tmin;

    Vector3D &kDiffuse = scene.materials[intersectObject->material_id].diffuse;
    Vector3D &kAmbient = scene.materials[intersectObject->material_id].ambient;
    Vector3D &kSpecular =
        scene.materials[intersectObject->material_id].specular;
    Vector3D &kMirror = scene.materials[intersectObject->material_id].mirror;

    // ambient light
    pixelColor = kAmbient.multiply(scene.ambient_light);

    for (parser::PointLight &light : scene.point_lights) {

      Vector3D wi = (light.position - intersectPoint).normalize();

      Vector3D shadowRayOrigin = intersectPoint + wi * scene.shadow_ray_epsilon;

      float stmin = FLOAT_MAX;

      // TODO OPTIMIZE NORMALIZATION

      for (auto &sobject : scene.objects) {
        float st = FLOAT_MAX;
        Vector3D tmp;
        if (sobject->intersects(shadowRayOrigin, wi, st, tmp, true) &&
            st < stmin + 0.0001) {
          stmin = st;
          // TODO: OPTIMIZE: NO NEED TO FIND CLOSER ONE!
        }
      }
      // FIXME TODO
      if (shadowRayOrigin.distance(light.position) < stmin + 0.0001) {
        // std::cout << shadowRayOrigin.distance(light.position) << " "
        //           << stmin << std::endl;

        Vector3D halfVector =
            (wi + direction.inverse()).normalize(); // for specular

        float costheta_diff = std::max(float(0), wi.dotProduct(normal));
        float distance2 = pow((intersectPoint).distance(light.position), 2);

        // diffuse shading
        pixelColor =
            pixelColor +
            (kDiffuse * costheta_diff).multiply(light.intensity / distance2);

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
    if ((kMirror.x > 0.001 || kMirror.y > 0.001 || kMirror.z > 0.001) &&
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

  int i = startHeight * imageWidth * 3;

  for (int y = startHeight; y < endHeight; ++y) {
    for (int x = 0; x < imageWidth; ++x) {
      // std::cout << x << " " << y << std::endl;

      Vector3D cameraPosition(camera->position.x, camera->position.y,
                              camera->position.z);
      Vector3D cameraGaze(camera->gaze.x, camera->gaze.y, camera->gaze.z);
      Vector3D cameraUp(camera->up.x, camera->up.y, camera->up.z);
      Vector3D cameraRight = cameraGaze * cameraUp; // cross product TODO
      // std::cout << "cameraRight: " << cameraRight.x << " " << cameraRight.y
      // << " " << cameraRight.z << std::endl;
      float pixelPositionX = (camera->near_plane.y - camera->near_plane.x) *
                             (x + 0.5) / imageWidth; // su
      float pixelPositionY = (camera->near_plane.w - camera->near_plane.z) *
                             (y + 0.5) / imageHeight; // sv
      // std::cout << "PIXEL: " << pixelPositionX << " " << pixelPositionY <<
      // std::endl;
      Vector3D centerOfPlane =
          cameraPosition + cameraGaze * camera->near_distance;
      // std::cout << "centerOfPlane: " << centerOfPlane.x << " " <<
      // centerOfPlane.y << " " << centerOfPlane.z << std::endl;
      Vector3D planeStartPoint = centerOfPlane +
                                 (cameraRight * camera->near_plane.x) +
                                 (cameraUp * camera->near_plane.w);
      // std::cout << "planeStartPoint: " << planeStartPoint.x << " " <<
      // planeStartPoint.y << " " << planeStartPoint.z << std::endl;

      Vector3D pixelPosition = planeStartPoint +
                               (cameraRight * pixelPositionX) -
                               (cameraUp * pixelPositionY);
      // std::cout << "pixelPosition: " << pixelPosition.x << " " <<
      // pixelPosition.y << " " << planeStartPoint.z << std::endl;
      Vector3D direction = pixelPosition - cameraPosition;
      // std::cout << "DIR: " << direction.x << " " << direction.y << " " <<
      // direction.z << std::endl;
      direction.normalize();

      Vector3D pixelColor = shade(*scene, cameraPosition, direction, 0);

      // std::cout << "i: " << i << " - " << pixelColor.x << std::endl;
      image[i++] = pixelColor.x; // r
      image[i++] = pixelColor.y; // g
      image[i++] = pixelColor.z; // b
    }
  }
}
/*
void createThread(parser::Scene &scene, parser::Camera &camera, int startHeight,
                  int endHeight, int imageWidth, int imageHeight,
                  unsigned char *image) {
  using std::cout;
  using std::endl;
  using std::thread;

  thread t1(trace, std::ref(scene), std::ref(camera), startHeight, endHeight,
            imageWidth, imageHeight, image);

  if (t1.joinable()) {
    t1.join();
    cout << "Thread with id " << t1.get_id() << " is terminated" << endl;
  }
}*/

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
    /*
void trace(parser::Scene &scene, parser::Camera &camera, int startHeight,
           int endHeight, int imageWidth, int imageHeight,
           unsigned char *image) {

*/
    int partition = height / 4;
    int startHeight = 0;
    int endHeight = partition;
    thread t1(trace, &scene, &camera, startHeight, endHeight, width, height,
              image);
    startHeight = endHeight;
    endHeight += partition;
    thread t2(trace, &scene, &camera, startHeight, endHeight, width, height,
              image);
    startHeight = endHeight;
    endHeight += partition;
    thread t3(trace, &scene, &camera, startHeight, endHeight, width, height,
              image);
    startHeight = endHeight;
    endHeight = height;
    thread t4(trace, &scene, &camera, startHeight, endHeight, width, height,
              image);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // trace(&scene, &camera, startHeight, endHeight, width, height, image1);
    // startHeight = endHeight;
    // endHeight += partition;
    // trace(&scene, &camera, startHeight, endHeight, width, height, image2);
    // startHeight = endHeight;
    // endHeight += partition;
    // trace(&scene, &camera, startHeight, endHeight, width, height, image3);
    // startHeight = endHeight;
    // endHeight = height;
    // trace(&scene, &camera, startHeight, endHeight, width, height, image4);

    /*
for (size_t i = 0; i < 4; i++) {

thread t1(trace, std::ref(scene), std::ref(camera), startHeight,
      endHeight, width, height, image);

// TODO MOVE THIS TO FUNCTION ABOVE

if (t1.joinable()) {
t1.join();
cout << "Thread is terminated" << endl;


// createThread(scene, camera, startHeight, endHeight, width, height,
//             image);
}
startHeight = endHeight;
endHeight += partition;
}*/

    write_ppm((camera.image_name).c_str(), image, width, height);
  }
}
