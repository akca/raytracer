#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include "face.h"
#include "object.h"
#include "vector3d.h"
#include <string>
#include <vector>

namespace parser {
// Notice that all the structures are as simple as possible
// so that you are not enforced to adopt any style or design.

struct Vec3i {
  int x, y, z;
};

struct Vec4f {
  float x, y, z, w;
};

struct Camera {
  Vector3D position;
  Vector3D gaze;
  Vector3D up;
  Vector3D right;
  Vector3D centerOfPlane;
  Vector3D planeStartPoint;
  Vec4f near_plane;
  float near_distance;
  int image_width, image_height;
  std::string image_name;
};

struct PointLight {
  Vector3D position;
  Vector3D intensity;
};

struct Material {
  Vector3D ambient;
  Vector3D diffuse;
  Vector3D specular;
  Vector3D mirror;
  float phong_exponent;
};
/*
        struct Face
        {
                int v0_id;
                int v1_id;
                int v2_id;
        };
*/
/*
        struct Mesh
        {
                int material_id;
                std::vector<Face> faces;
        };

        struct Triangle
        {
                int material_id;
                Face indices;
        };

        struct Sphere
        {
                int material_id;
                int center_vertex_id;
                float radius;
        };*/

struct Scene {
  // Data
  Vec3i background_color;
  float shadow_ray_epsilon;
  int max_recursion_depth;
  std::vector<Camera> cameras;
  Vector3D ambient_light;
  std::vector<PointLight> point_lights;
  std::vector<Material> materials;
  std::vector<Vector3D> vertex_data;
  std::vector<Object *> objects;

  // Functions
  void loadFromXml(const std::string &filepath);
};
} // namespace parser

#endif
